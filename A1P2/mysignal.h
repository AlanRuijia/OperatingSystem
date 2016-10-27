#ifndef MYSIGNAL_H
#define MYSIGNAL_H

#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include "job.h"

void sig_int();
void sig_stp();
void sig_quit();

#endif