DEFINES += -D LINUX
DEFINES += -D REPORT_SYSLOG
#DEFINES += -D REPORT_DEBUG
#DEFINES += -D REPORT_DUMP

CC := gcc
CPP := g++
LINK := g++

INCLUDES += -I ../
INCLUDES += -I /usr/include/postgresql/

LIBS += -L../APNS/
LIBS += -L../Communicator/
LIBS += -L../HTTP/
LIBS += -L../RTSP/
LIBS += -L../PostgreSQL/
LIBS += -L../Toolkit/

LIBS += -lAPNS
LIBS += -lCryptography
LIBS += -lHTTP
LIBS += -lMMPS
LIBS += -lPGSQL
LIBS += -lRTSP
LIBS += -lSignals
LIBS += -lToolkit
LIBS += -lCommunicator
LIBS += -pthread
LIBS += -lconfig++
LIBS += -lbz2
LIBS += -lm
LIBS += -lpq
LIBS += -lrt
LIBS += -lssl
LIBS += -lcrypto

CFLAGS += -O2
CFLAGS += -std=c99
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -Wbad-function-cast
CFLAGS += -Wdeprecated-declarations
CFLAGS += -Wimplicit-function-declaration
CFLAGS += -Wpointer-sign
CFLAGS += -Wsign-compare
CFLAGS += -Wtype-limits
CFLAGS += -Wunknown-pragmas
CFLAGS += -Wunused-but-set-variable
CFLAGS += -Wunused-parameter
CFLAGS += -Wunused-value
CFLAGS += -Wwrite-strings

CPPFLAGS += -O2
CPPFLAGS += -std=c++14
CPPFLAGS += -Wall
CPPFLAGS += -Wextra
CPPFLAGS += -Wdeprecated-declarations
CPPFLAGS += -Wsign-compare
CPPFLAGS += -Wtype-limits
CPPFLAGS += -Wunknown-pragmas
CPPFLAGS += -Wunused-but-set-variable
CPPFLAGS += -Wunused-parameter
CPPFLAGS += -Wunused-value
CPPFLAGS += -Wwrite-strings

# ******************************************************************************

OBJECTS_ROOT        := Configuration.o Kernel.o Main.o Parse.o
OBJECTS_DATABASE    := Database/Activator.o Database/Activators.o Database/Database.o Database/Debug.o Database/Phoenix.o Database/Phoenixes.o Database/Relay.o Database/Relays.o Database/Servus.o Database/Servuses.o Database/Therma.o Database/Thermas.o
OBJECTS_DISPATCHER  := Dispatcher/Fabula.o Dispatcher/Fabulas.o Dispatcher/Listener.o Dispatcher/Notificator.o Dispatcher/Session.o
OBJECTS_ANTICIPATOR := Anticipator/Listener.o Anticipator/Service.o Anticipator/Session.o
OBJECTS_WWW         := WWW/Activator.o WWW/Home.o WWW/Phoenix.o WWW/Relay.o WWW/Servus.o WWW/SessionManager.o WWW/SystemInformation.o WWW/Therma.o

all: Primus

Primus: $(OBJECTS_ROOT) $(OBJECTS_APNS) $(OBJECTS_DATABASE) $(OBJECTS_DISPATCHER) $(OBJECTS_ANTICIPATOR) $(OBJECTS_WWW)
	$(LINK) $(LINKFLAGS) -o $@ $^ $(LIBS)

# ******************************************************************************

Configuration.o: Configuration.cpp
	$(CPP) -c $(CPPFLAGS) $(INCLUDES) $(DEFINES) $< -o $@

Kernel.o: Kernel.cpp
	$(CPP) -c $(CPPFLAGS) $(INCLUDES) $(DEFINES) $< -o $@

Main.o: Main.cpp
	$(CPP) -c $(CPPFLAGS) $(INCLUDES) $(DEFINES) $< -o $@

Parse.o: Parse.cpp
	$(CPP) -c $(CPPFLAGS) $(INCLUDES) $(DEFINES) $< -o $@

# ******************************************************************************

Database/Activator.o: Database/Activator.cpp
	$(CPP) -c $(CPPFLAGS) $(INCLUDES) $(DEFINES) $< -o $@

Database/Activators.o: Database/Activators.cpp
	$(CPP) -c $(CPPFLAGS) $(INCLUDES) $(DEFINES) $< -o $@

Database/Database.o: Database/Database.cpp
	$(CPP) -c $(CPPFLAGS) $(INCLUDES) $(DEFINES) $< -o $@

Database/Debug.o: Database/Debug.cpp
	$(CPP) -c $(CPPFLAGS) $(INCLUDES) $(DEFINES) $< -o $@

Database/Phoenix.o: Database/Phoenix.cpp
	$(CPP) -c $(CPPFLAGS) $(INCLUDES) $(DEFINES) $< -o $@

Database/Phoenixes.o: Database/Phoenixes.cpp
	$(CPP) -c $(CPPFLAGS) $(INCLUDES) $(DEFINES) $< -o $@

Database/Relay.o: Database/Relay.cpp
	$(CPP) -c $(CPPFLAGS) $(INCLUDES) $(DEFINES) $< -o $@

Database/Relays.o: Database/Relays.cpp
	$(CPP) -c $(CPPFLAGS) $(INCLUDES) $(DEFINES) $< -o $@

Database/Servus.o: Database/Servus.cpp
	$(CPP) -c $(CPPFLAGS) $(INCLUDES) $(DEFINES) $< -o $@

Database/Servuses.o: Database/Servuses.cpp
	$(CPP) -c $(CPPFLAGS) $(INCLUDES) $(DEFINES) $< -o $@

Database/Therma.o: Database/Therma.cpp
	$(CPP) -c $(CPPFLAGS) $(INCLUDES) $(DEFINES) $< -o $@

Database/Thermas.o: Database/Thermas.cpp
	$(CPP) -c $(CPPFLAGS) $(INCLUDES) $(DEFINES) $< -o $@

# ******************************************************************************

Dispatcher/Fabula.o: Dispatcher/Fabula.cpp
	$(CPP) -c $(CPPFLAGS) $(INCLUDES) $(DEFINES) $< -o $@

Dispatcher/Fabulas.o: Dispatcher/Fabulas.cpp
	$(CPP) -c $(CPPFLAGS) $(INCLUDES) $(DEFINES) $< -o $@

Dispatcher/Listener.o: Dispatcher/Listener.cpp
	$(CPP) -c $(CPPFLAGS) $(INCLUDES) $(DEFINES) $< -o $@

Dispatcher/Notificator.o: Dispatcher/Notificator.cpp
	$(CPP) -c $(CPPFLAGS) $(INCLUDES) $(DEFINES) $< -o $@

Dispatcher/Session.o: Dispatcher/Session.cpp
	$(CPP) -c $(CPPFLAGS) $(INCLUDES) $(DEFINES) $< -o $@

# ******************************************************************************

Anticipator/Listener.o: Anticipator/Listener.cpp
	$(CPP) -c $(CPPFLAGS) $(INCLUDES) $(DEFINES) $< -o $@

Anticipator/Service.o: Anticipator/Service.cpp
	$(CPP) -c $(CPPFLAGS) $(INCLUDES) $(DEFINES) $< -o $@

Anticipator/Session.o: Anticipator/Session.cpp
	$(CPP) -c $(CPPFLAGS) $(INCLUDES) $(DEFINES) $< -o $@

# ******************************************************************************

WWW/Activator.o: WWW/Activator.cpp
	$(CPP) -c $(CPPFLAGS) $(INCLUDES) $(DEFINES) $< -o $@

WWW/Home.o: WWW/Home.cpp
	$(CPP) -c $(CPPFLAGS) $(INCLUDES) $(DEFINES) $< -o $@

WWW/Relay.o: WWW/Relay.cpp
	$(CPP) -c $(CPPFLAGS) $(INCLUDES) $(DEFINES) $< -o $@

WWW/Phoenix.o: WWW/Phoenix.cpp
	$(CPP) -c $(CPPFLAGS) $(INCLUDES) $(DEFINES) $< -o $@

WWW/Servus.o: WWW/Servus.cpp
	$(CPP) -c $(CPPFLAGS) $(INCLUDES) $(DEFINES) $< -o $@

WWW/SessionManager.o: WWW/SessionManager.cpp
	$(CPP) -c $(CPPFLAGS) $(INCLUDES) $(DEFINES) $< -o $@

WWW/SystemInformation.o: WWW/SystemInformation.cpp
	$(CPP) -c $(CPPFLAGS) $(INCLUDES) $(DEFINES) $< -o $@

WWW/Therma.o: WWW/Therma.cpp
	$(CPP) -c $(CPPFLAGS) $(INCLUDES) $(DEFINES) $< -o $@

# ******************************************************************************

install:
	sudo install --owner=root --group=root --mode=0755 Primus.rc /etc/init.d/primus
	sudo install --owner=root --group=root --mode=0755 --directory /opt/castellum/
	sudo install --owner=root --group=root --mode=0755 --preserve-timestamps --strip Primus /opt/castellum/primus

setup:
	sudo install --owner=root --group=root --mode=0644 --preserve-timestamps Default.conf /opt/castellum/primus.conf

clean:
	rm -fv Primus
	find . -type f -name "*.o" | xargs rm -fv *.o

run:
	sudo killall --quiet --wait primus | echo "Stopped"
	sudo /opt/castellum/primus | echo "Started"
