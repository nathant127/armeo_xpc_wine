#include "xpc/include/xpcapi.h"
#include "xpc/include/xpcapiconst.h"

#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

volatile bool keepRunning = true;

BOOL WINAPI ConsoleHandler(DWORD dwCtrlType) {
    switch (dwCtrlType) {
        case CTRL_C_EVENT:
            printf("\n[Handler] Ctrl+C detected! Cleaning up...\n");
            keepRunning = false;
            
            // Return TRUE to tell Windows we handled the event.
            // If you return FALSE, the next registered handler is called, 
            // and if none handle it, the process terminates.
            return TRUE;

        case CTRL_CLOSE_EVENT:
            printf("\n[Handler] Console window is closing!\n");
            keepRunning = false;
            return TRUE;

        default:
            // Pass other events (like logoff/shutdown) to the system default
            return FALSE;
    }
}


int xpcError() {
  const int err = xPCGetLastError();
  if (err == ENOERR) return 0;

  const char *err_msg = xPCErrorMsg(err, NULL);
  fprintf(stderr, "xPC error %d: %s\n", err, err_msg);
  return err;
}

char * get_cur_dir() {
    char* cwd = malloc(MAX_PATH);

    // Retrieve the current directory
    DWORD buffer_len = GetCurrentDirectory(MAX_PATH, cwd);

    if (buffer_len == 0) {
        printf("Failed to get current directory. Error code: %lu\n", GetLastError());
        return NULL;
    }

    return cwd;
}

int main(int argc, char** argv) {
    if (!SetConsoleCtrlHandler(ConsoleHandler, TRUE)) {
        fprintf(stderr, "ERROR: Could not set control handler\n");
        return 1;
    }

    if (xPCInitAPI("../dll/xpcapi.dll") == -1) {
        fprintf(stderr, "Failed to load .dll\n");
        return 1;
    }
    
    int port = xPCOpenTcpIpPort("10.10.10.11", "22222");

    if(xpcError()) {
        fprintf(stderr, "Failed to open tcpip port\n");
        return 1;
    }

    char * dir = get_cur_dir();
    strcat(dir, "\\xpc");
    char fname[] = "AM2_3_1_01HWfullV2_PCIe";
    xPCLoadApp(port, dir, fname);
    if(xpcError()) {
        fprintf(stderr, "Failed to load app %s\\%s\n", dir, fname);
        return 1;
    }

    if (!xPCIsAppRunning(port)) {
        xPCStartApp(port);
        printf("xPC started.\n");
    } else {
        fprintf(stderr, "App was already running? Continuing anyway\n");
    }

    // Do something

    int num_sig = xPCGetNumSignals(port);
    int num_par = xPCGetNumParams(port);
    printf("There are %i signals and %i parameters\n", num_sig, num_par);

    // ee_pos
    // char sig_x_name[] = "Output/SoftwareSignals/EndEffector/s1";
    // char sig_y_name[] = "Output/SoftwareSignals/EndEffector/s2";
    // char sig_z_name[] = "Output/SoftwareSignals/EndEffector/s3";

    // int sig_x_idx = xPCGetSignalIdx(port, sig_x_name);
    // int sig_y_idx = xPCGetSignalIdx(port, sig_y_name);
    // int sig_z_idx = xPCGetSignalIdx(port, sig_z_name);

    // printf("The signal ids of xyz are %i, %i, %i\n", sig_x_idx, sig_y_idx, sig_z_idx);

    // Sleep(2000);
    // while (keepRunning) {
    //     // double x = xPCGetSignal(port, sig_x_idx);
    //     // double y = xPCGetSignal(port, sig_y_idx);
    //     // double z = xPCGetSignal(port, sig_y_idx);

    //     // printf("xyz: %.2f, %.2f, %.2f\n", x, y, z);

    //     // Sleep(1000); 
    // }
    
    // FILE *fptr;
    // fptr = fopen("param_names.txt", "w");
    // char block[1024];
    // char param[1024];
    // fprintf(fptr, "ID,BLOCK,PARAM");
    // for (int i = 0; i < num_par; i++) {
    //     xPCGetParamName(port, i, block, param);
    //     fprintf(fptr, "%i,%s,%s\n", i, block, param);
    // }

    // fclose(fptr);
    
    // FILE *fptr;
    // fptr = fopen("signal_names.txt", "w");
    // char name[1024];
    // fprintf(fptr, "ID,NAME\n");
    // for (int i = 0; i < num_sig; i++) {
    //     xPCGetSignalName(port, i, name);
    //     fprintf(fptr, "%i,%s\n", i, name);
    // }

    // fclose(fptr);

    //141,Input/OperationMode/OperationMode,Value
    double OM;
    xPCGetParam(port, 141, &OM);
    printf("Current Operation Mode = %f\n", OM);

    //4035,,OMDirectTorque
    double OMDirectTorque;
    xPCGetParam(port, 4035, &OMDirectTorque);
    printf("OMDirectTorque Val = %f\n", OMDirectTorque);


    // Set operation mode to Direct torque
    xPCSetParam(port, 141, &OMDirectTorque);

    xPCGetParam(port, 141, &OM);
    if ((int)OM != (int)OMDirectTorque) {
        fprintf(stderr, "Failed to set operation mode");
    }

    /*
        467,Input/OperationMode/DirectTorqueMode/Torque1,Value
        468,Input/OperationMode/DirectTorqueMode/Torque2,Value
        469,Input/OperationMode/DirectTorqueMode/Torque3,Value
        470,Input/OperationMode/DirectTorqueMode/Torque4,Value
        471,Input/OperationMode/DirectTorqueMode/Torque5,Value
        472,Input/OperationMode/DirectTorqueMode/Torque6,Value
        473,Input/OperationMode/DirectTorqueMode/Torque7,Value
    */
    double torque = 0.01;
    xPCSetParam(port, 470, &torque);

    Sleep(2000);






    if (xPCIsAppRunning(port)) {
        xPCStopApp(port);
    }
    xPCUnloadApp(port);
}
