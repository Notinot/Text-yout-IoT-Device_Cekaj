APPLICATION = paho-mqtt-example

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 to show compiler invocation lines by default:
QUIET ?= 1

WIFI_SSID ?= "Your_WiFi_name"
WIFI_PASS ?= "Your_secure_password"

# Include required modules
USEMODULE += shell_cmds_default
USEMODULE += ps
USEMODULE += netdev_default
USEPKG += paho-mqtt

# paho-mqtt depends on TCP support, choose which stacks you want
GNRC_IPV6 ?= 1
LWIP_IPV4 ?= 0
LWIP_IPV6 ?= 0

include Makefile.lwip
include Makefile.gnrc

USEMODULE += sock_async_event
USEMODULE += sock_ip
USEMODULE += sock_udp
USEMODULE += sock_tcp



USEMODULE += ztimer
USEMODULE += ztimer_sec
USEMODULE += ztimer_msec

# Ensure SAUL support is included
USEMODULE += saul
USEMODULE += saul_default



####

include $(RIOTBASE)/Makefile.include

ifneq (,$(filter arch_esp,$(FEATURES_USED)))
  CFLAGS += -DWIFI_SSID=\"$(WIFI_SSID)\"
  CFLAGS += -DWIFI_PASS=\"$(WIFI_PASS)\"
endif 
