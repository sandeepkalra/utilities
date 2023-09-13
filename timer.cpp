/* Implementation of Differential Timer */
#include <algorithm>
#include <chrono>
#include <iostream>
#include <list>
#include <memory>
#include <stdio.h>
#include <thread>
#include <time.h>
using namespace std;
typedef void (*callback)(int);

struct timer_element {
  int unique_user_id;
  int unique_timer_id;
  long long ms_wait;
  callback cb;
  bool valid;
  int arg_to_cb;
};

list<timer_element *> diff_timer;

void invalidate(int user_id, int unique_timer_id) {
  /* TODO: The code used to compile
  auto it =
      find_if(diff_timer.begin(), diff_timer.end(), [&](timer_element *p) {
        return ((p->unique_user_id == user_id) &&
                (p->unique_timer_id == unique_timer_id));
      });
  if (it != diff_timer.end()) {
    (*it)->valid = false;
  }
  */
}

int insert(int user_id, long long waittime, callback cb, int arg_of_cb) {
  if (diff_timer.empty()) {
    timer_element *p = new timer_element();
    p->unique_user_id = user_id;
    p->unique_timer_id = 0;
    p->cb = cb;
    p->ms_wait = waittime;
    p->arg_to_cb = arg_of_cb;
    p->valid = true;
    diff_timer.push_front(p);
    return p->unique_timer_id;
  } else {
    list<timer_element *>::iterator it = diff_timer.begin();
    long long time_left = waittime;
    int last_good_unique_timer_id = 0;

    while (it != diff_timer.end()) {
      if (waittime < (*it)->ms_wait) {
        // insert before this.
        (*it)->ms_wait -= time_left;
        timer_element *p = new timer_element();
        p->ms_wait = time_left;
        p->unique_user_id = user_id;
        p->unique_timer_id = (*it)->unique_timer_id + 1;
        p->cb = cb;
        p->valid = true;
        p->arg_to_cb = arg_of_cb;
        diff_timer.insert(it, 1, p);
        return p->unique_timer_id;
      } else {
        last_good_unique_timer_id = (*it)->unique_timer_id;
        time_left -= (*it)->ms_wait;
        it++;
      }
    }
    // we are here, that means the whole list has exhausted, and we still
    // havent reached the right place to emplace this. so , insert at end.
    timer_element *p = new timer_element();
    p->ms_wait = time_left;
    p->unique_user_id = user_id;
    p->unique_timer_id = last_good_unique_timer_id + 1;
    p->cb = cb;
    p->valid = true;
    p->arg_to_cb = arg_of_cb;
    diff_timer.push_back(p);
    return p->unique_timer_id;
  } // else

  return -1;
} // function insert!

static bool run = true;
void process_thread() {
  while (run) {
    if (diff_timer.empty()) {
      this_thread::sleep_for(chrono::milliseconds(100));
    } else {
      list<timer_element *>::iterator it = diff_timer.begin();
      if ((*it)->ms_wait > 100) {
        (*it)->ms_wait -= 100;
        this_thread::sleep_for(chrono::milliseconds(100));
      } else {
        this_thread::sleep_for(chrono::milliseconds((*it)->ms_wait));
        callback cb = (*it)->cb;
        int arg = (*it)->arg_to_cb;
        bool valid = (*it)->valid;
        if (valid)
          cb(arg);
        diff_timer.erase(it);
      }
    }
  }
}

void timenow() {
  auto now = chrono::system_clock::now();
  time_t time_now = chrono::system_clock::to_time_t(now);
  cout << "executed at " << ctime(&time_now);
}

void f(int i) {
  timenow();
  cout << "thread " << i << endl;
}

int main() {
  printf("Thread started!\n");
  timenow();
  cout << "inserting 1, 5000 ms, 2 for 3010 ms, and 3 for 1100 ms" << endl;
  insert(1, 5000, f, 1);
  insert(2, 3010, f, 2);
  insert(3, 1100, f, 3);
  thread th(process_thread);
  th.join();
}
