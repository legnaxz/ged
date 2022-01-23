################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../CKeepAlivedServer.cpp 

OBJS += \
./CKeepAlivedServer.o 

CPP_DEPS += \
./CKeepAlivedServer.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DLINUX -DMY_TCP_DEBUG -DVAS_ENABLED -DCTI_ENABLED -DPFS_ENABLED -I"/media/sf_Temp/ged188_KT/ged188/include" -O3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


