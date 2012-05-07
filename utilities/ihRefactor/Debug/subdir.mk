################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../ihRefactor.cpp \
/home/stuart/sw-dev/inthex/intelhex_class/intelhexclass.cpp 

OBJS += \
./ihRefactor.o \
./intelhexclass.o 

CPP_DEPS += \
./ihRefactor.d \
./intelhexclass.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/stuart/sw-dev/inthex/intelhex_class -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

intelhexclass.o: /home/stuart/sw-dev/inthex/intelhex_class/intelhexclass.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/stuart/sw-dev/inthex/intelhex_class -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


