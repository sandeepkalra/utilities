package main

import (
	"flag"
	"fmt"
	"github.com/sirupsen/logrus"
	"io"
	"os"
	"time"
)

const (
	BUFF_READ_SIZE = 200
)

func main() {
	filename := flag.String("filename", "", "file to trail on")
	flag.Parse()
	if *filename == "" {
		logrus.Fatal("filename to trail on wasn't provided. Pls check usage")
		return
	}

	f, e := os.Open(*filename)
	if e != nil {
		logrus.Fatal("unable to open file ", *filename)
		return
	}
	defer f.Close()
	size, e := f.Seek(0, 2) /* seek the size of file */
	if e != nil {
		logrus.Fatal("unable to seek end of file")
		return
	}
	offset := int64(0)
	logrus.Info("file is of size ", size)
	if size > BUFF_READ_SIZE {
		/* going back BUFF_READ_SIZE to start reading from */
		offset = size - int64(BUFF_READ_SIZE)
	}
	for {
		b := make([]byte, BUFF_READ_SIZE+1)
		if n, err := f.ReadAt(b, offset); err == nil {
			if n > 0 {
				fmt.Print(string(b))
				offset = offset + int64(n)
			}
		} else if err == io.EOF {
			/* we read n, and wait for some time to try again */
			if n > 0 {
				fmt.Print(string(b))
				offset = offset + int64(n)
			}
			time.Sleep(50 * time.Millisecond)
		}
	}
}
