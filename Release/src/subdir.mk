################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/CKeepAlivedClient.cpp \
../src/agentstate.cpp \
../src/cfg.cpp \
../src/ctiifhandler.cpp \
../src/ctiqueue.cpp \
../src/ctisocketif.cpp \
../src/log.cpp \
../src/main.cpp \
../src/messagetype.cpp \
../src/mysocket.cpp \
../src/mythread.cpp \
../src/pfsifhandler.cpp \
../src/pgstatuscode.cpp \
../src/pktdef.cpp \
../src/posixsem.cpp \
../src/statuscode.cpp \
../src/tagvalue.cpp \
../src/tcpserver.cpp \
../src/util.cpp \
../src/vasextra.cpp \
../src/vasifhandler.cpp 

OBJS += \
./src/CKeepAlivedClient.o \
./src/agentstate.o \
./src/cfg.o \
./src/ctiifhandler.o \
./src/ctiqueue.o \
./src/ctisocketif.o \
./src/log.o \
./src/main.o \
./src/messagetype.o \
./src/mysocket.o \
./src/mythread.o \
./src/pfsifhandler.o \
./src/pgstatuscode.o \
./src/pktdef.o \
./src/posixsem.o \
./src/statuscode.o \
./src/tagvalue.o \
./src/tcpserver.o \
./src/util.o \
./src/vasextra.o \
./src/vasifhandler.o 

CPP_DEPS += \
./src/CKeepAlivedClient.d \
./src/agentstate.d \
./src/cfg.d \
./src/ctiifhandler.d \
./src/ctiqueue.d \
./src/ctisocketif.d \
./src/log.d \
./src/main.d \
./src/messagetype.d \
./src/mysocket.d \
./src/mythread.d \
./src/pfsifhandler.d \
./src/pgstatuscode.d \
./src/pktdef.d \
./src/posixsem.d \
./src/statuscode.d \
./src/tagvalue.d \
./src/tcpserver.d \
./src/util.d \
./src/vasextra.d \
./src/vasifhandler.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DLINUX -DMY_TCP_DEBUG -DVAS_ENABLED -DCTI_ENABLED -DPFS_ENABLED -I"/media/sf_Temp/ged188_KT/ged188/include" -O3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


