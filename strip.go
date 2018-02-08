// Copyright(c) Sandeep Kalra

package main

import (
	"bufio"
	"fmt"
	"os"
	"strings"
)

const (
	SyslogNL     = "" // "^M"
	MyIn         = "=IN="
	MyOut        = "=OUT="
	TokenRxTx    = "" // "[IP_RX_TX_Z]:"
	TokenOut     = "" // "=Out=>>>"
	TokenIn      = "" // "<<<=In="
	TokenRxTxLen = len(TokenRxTx)
	TokenOutLen  = len(TokenOut)
	TokenInLen   = len(TokenIn)
)

func saveFile(outFile string, outChanString chan string, inControlChan chan int) {
	f, err := os.Create(outFile)
	if err != nil {
		fmt.Println("Failed to create file : ", outFile, " Error :", err.Error())
	}
	defer f.Close()
	for {
		select {
		case inString := <-outChanString:
			f.WriteString(inString)
			f.WriteString("\r\n")
		case <-inControlChan:
			return
		}
	}
}
func readFile(inStringChan, outStringChan chan string, inControlChan chan int) {
	for {
		select {
		case inString := <-inStringChan:
			b := []byte(inString)
			b = b[50:]
			s := string(b)

			if indx := strings.Index(s, TokenRxTx); indx != -1 {
				s = s[TokenRxTxLen:]
			}

			if indx := strings.Index(s, TokenOut); indx != -1 {
				outStringChan <- MyOut
				s = s[TokenOutLen:]
			}

			if indx := strings.Index(s, TokenIn); indx != -1 {
				outStringChan <- MyIn
				s = s[TokenInLen:]
			}

			if indx := strings.Index(s, SyslogNL); indx != -1 {
				ss := strings.Split(s, SyslogNL)
				for _, v := range ss {
					outStringChan <- v
				}
			} else {
				outStringChan <- s
			}
		case inCmd := <-inControlChan:
			fmt.Println("got Control ", inCmd)
			return
		}
	}
}

func openFile(file string, outChan chan string) error {
	f, err := os.Open(file)
	if err != nil {
		return err
	}
	defer f.Close()

	fscanner := bufio.NewScanner(f)
	for fscanner.Scan() {
		if strings.Contains(fscanner.Text(), "timp_voip") {
			outChan <- fscanner.Text()
		}
	}

	return nil
}

func main() {
	if len(os.Args) != 3 {
		fmt.Println("Usage: ParseToken <input_log_file> <output_file_name>")
		fmt.Println("got arguments count ", len(os.Args))
		return
	}
	cIn := make(chan string)
	cOut := make(chan string)
	i := make(chan int, 2)
	go readFile(cIn, cOut, i)
	go saveFile(os.Args[2], cOut, i)
	if e := openFile(os.Args[1], cIn); e != nil {
		fmt.Println("readFile error, e=", e.Error())
	}
	i <- 0
	i <- 0
}
