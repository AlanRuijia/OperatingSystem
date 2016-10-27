#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void proc_parse(){
	
	FILE *fCpu, *fKernel, *fMem, *fTime;
	char info[500];
	int i;

	fCpu = fopen("/proc/cpuinfo","r");
	for (i=0;i<30;i++){
		fgets(info, 90, fCpu);
		if (!strncmp(info,"model name",10)){
			printf("Proccessor Type : %s", &info[13]);
			break;
		}
	}

	fKernel = fopen("/proc/version", "r");
	fgets(info,300, fKernel);
	printf("Kernel version: %s", info);

	fMem = fopen("/proc/meminfo","r");
	for (i=0;i<30;i++){
		// fscanf(fCpu, "%s", info);
		fgets(info, 200, fMem);
		if (!strncmp(info,"MemTotal",8)){
			printf("The amount of memory configured into this computer : %s", &info[17]);
			break;
		}
	}

	fTime = fopen("/proc/uptime", "r");
	fscanf(fTime,"%s", info);
	printf("Amount of time since the system was last booted: %s Seconds\n",info);

	fclose(fCpu);
	fclose(fKernel);
	fclose(fMem);
	fclose(fTime);
	return;

}

int main(){

	proc_parse();

}