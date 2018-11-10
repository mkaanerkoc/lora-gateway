CC            = gcc
CFLAGS        = -DRASPBERRY_PI -DGATEWAY_CODE -DBCM2835_NO_DELAY_COMPATIBILITY -D__BASEFILE__=\"$*\" 
CFLAGSDEBUG   = -DRASPBERRY_PI -DBCM2835_NO_DELAY_COMPATIBILITY -D__BASEFILE__=\"$*\" -DDEBUG_CONSOLE
MODULES_BASE=Modules
MYSQL= /usr/include/mysql
MONGOOSE=libraries/mongoose
COMMON = Modules/Common
REMOTE_CONTROL=Modules/RemoteControl
INC = 	-I$(MODULES_BASE) \
		-I$(MODULES_BASE)/Storage -I$(MODULES_BASE)/Radio \
		-I$(MODULES_BASE)/Modbus -I$(MODULES_BASE)/Utils -I$(MONGOOSE) \
		-I$(REMOTE_CONTROL) -I$(MYSQL) -I$(COMMON)
LIBS=   -lpthread -lrt -lmysqlclient -L/usr/lib/mysql -lbcm2835

gateway: main.o DataChannelManager.o StorageManager.o mongoose.o EthernetControl.o DeviceManager.o EEPROMModule.o RadioManager.o  RoutingManager.o SX1272Driver.o Radio_QueueManager.o Radio_Services.o ModbusTCPSlave.o RadioManager_Interface.o ModbusCore.o RTCManager.o TimerModule.o EthernetControl_Handler.o 
	$(CC)  $^ -o gateway $(LIBS)

main.o: main.c 
	$(CC) $(CFLAGS) $(INC) -c main.c $(LIBS)

ModbusCore.o: $(MODULES_BASE)/Modbus/ModbusCore.c
	$(CC) $(CFLAGS) $(INC) -c $(MODULES_BASE)/Modbus/ModbusCore.c

	
ModbusTCPSlave.o: $(MODULES_BASE)/Modbus/ModbusTCPSlave.c
	$(CC) $(CFLAGS) $(INC) -c $(MODULES_BASE)/Modbus/ModbusTCPSlave.c
	
RadioManager.o: $(MODULES_BASE)/Radio/RadioManager.c  \
				$(MODULES_BASE)/Radio/SX1272Driver.c 
	$(CC) $(CFLAGS) $(INC) -c $(MODULES_BASE)/Radio/RadioManager.c	$< $(LIBS)
	
RoutingManager.o: $(MODULES_BASE)/Radio/RoutingManager.c
	$(CC) $(CFLAGS) $(INC) -c $(MODULES_BASE)/Radio/RoutingManager.c $< $(LIBS)


RadioManager_Interface.o: $(MODULES_BASE)/Radio/RadioManager_Interface.c
	$(CC) $(CFLAGS) $(INC) -c $(MODULES_BASE)/Radio/RadioManager_Interface.c $< $(LIBS)
	
Radio_QueueManager.o: $(MODULES_BASE)/Radio/Radio_QueueManager.c
	$(CC) $(CFLAGS) $(INC) -c $(MODULES_BASE)/Radio/Radio_QueueManager.c $< $(LIBS)

Radio_Services.o: $(MODULES_BASE)/Radio/Radio_Services.c
	$(CC) $(CFLAGS) $(INC) -c $(MODULES_BASE)/Radio/Radio_Services.c $< $(LIBS)
	
SX1272Driver.o:	$(MODULES_BASE)/Radio/SX1272Driver.c
	$(CC) $(CFLAGS) $(INC) -c $(MODULES_BASE)/Radio/SX1272Driver.c	$< $(LIBS)
	
StorageManager.o: $(MODULES_BASE)/Storage/StorageManager.c $(MODULES_BASE)/Storage/StorageManager.h
	$(CC) $(CFLAGS) $(INC) -c $(MODULES_BASE)/Storage/StorageManager.c $<

EEPROMModule.o: $(MODULES_BASE)/Storage/EEPROMModule.c
	$(CC) $(CFLAGS) $(INC) -c $(MODULES_BASE)/Storage/EEPROMModule.c $<	

	
DeviceManager.o: $(MODULES_BASE)/DeviceManager.c 
	$(CC) $(CFLAGS) $(INC) -c $(MODULES_BASE)/DeviceManager.c $<


TimerModule.o: $(MODULES_BASE)/Utils/TimerModule.c
	$(CC) $(CFLAGS) $(INC) -c $(MODULES_BASE)/Utils/TimerModule.c $<

RTCManager.o: $(MODULES_BASE)/Utils/RTCManager.c
	$(CC) $(CFLAGS) $(INC) -c $(MODULES_BASE)/Utils/RTCManager.c $<
	
DataChannelManager.o: $(MODULES_BASE)/DataChannelManager.c $(MODULES_BASE)/DataChannelManager.h
	$(CC) $(CFLAGS) $(INC) -c $(MODULES_BASE)/DataChannelManager.c $<

EthernetControl.o: 	$(REMOTE_CONTROL)/EthernetControl.c  $(REMOTE_CONTROL)/EthernetControl_Handler.c
	$(CC) $(CFLAGS) $(INC) -c $(REMOTE_CONTROL)/EthernetControl.c $< $(LIBS) 
	
EthernetControl_Handler.o: 	$(REMOTE_CONTROL)/EthernetControl_Handler.c $(REMOTE_CONTROL)/EthernetControl_Interface.h 
	$(CC) $(CFLAGS) $(INC) -c $(REMOTE_CONTROL)/EthernetControl_Handler.c $< $(LIBS) 

mongoose.o: $(MONGOOSE)/mongoose.c $(MONGOOSE)/mongoose.h
	$(CC) $(CFLAGS) $(INC) -c $(MONGOOSE)/mongoose.c $<
	
clean:
	rm *.o gateway
