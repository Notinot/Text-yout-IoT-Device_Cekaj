/* Generated file do not edit */
#ifdef DOXYGEN
#define RIOT_VERSION "<YEAR_OF_RELEASE>.<MONTH_OF_RELEASE>-<POSTFIX>"
#define RIOT_VERSION_CODE   RIOT_VERSION_NUM(<YEAR>,<MONTH>,<PATCH>,<EXTRA>)
#define RIOT_APPLICATION    "<RIOT_APP_NAME>"
#define RIOT_BOARD          "<BOARD_NAME>"
#define RIOT_CPU            "<CPU_FOLDER_NAME>"
#define CPU_RAM_SIZE        /* RAM Size in Bytes */
#endif /* DOXYGEN */
#if defined(DOXYGEN)
#  define MACRO_DEPRECATED    /* implementation */
#elif defined(__GNUC__) || defined(__clang__)
#  define MACRO_DEPRECATED _Pragma("GCC warning \"Code is using a deprecated macro\"")
#else
#  define MACRO_DEPRECATED
#endif
#define RIOT_MCU        MACRO_DEPRECATED RIOT_CPU
#define DEVELHELP 1
#define CPU_FAM_NRF52 1
#define CPU_MODEL_NRF52840XXAA 1
#define CPU_CORE_CORTEX_M4F 1
#define RIOT_APPLICATION "paho-mqtt-example"
#define BOARD_NRF52840DK "nrf52840dk"
#define RIOT_BOARD BOARD_NRF52840DK
#define CPU_NRF52 "nrf52"
#define RIOT_CPU CPU_NRF52
#define CPU_RAM_BASE 0x20000000
#define CPU_RAM_SIZE 0x40000
#define SOCK_HAS_ASYNC 1
#define SOCK_HAS_IPV6 1
#define SOCK_HAS_ASYNC 1
#define SOCK_HAS_ASYNC_CTX 1
#define RTT_FREQUENCY RTT_MAX_FREQUENCY
#define MQTT_TASK 1
#define RIOT_VERSION "2025.01-devel-252-gd6f463"
#define RIOT_VERSION_CODE RIOT_VERSION_NUM(2025,01,0,0)
#define MODULE_AUTO_INIT 1
#define MODULE_AUTO_INIT_GNRC_IPV6 1
#define MODULE_AUTO_INIT_GNRC_IPV6_NIB 1
#define MODULE_AUTO_INIT_GNRC_NETIF 1
#define MODULE_AUTO_INIT_GNRC_PKTBUF 1
#define MODULE_AUTO_INIT_GNRC_SIXLOWPAN 1
#define MODULE_AUTO_INIT_GNRC_TCP 1
#define MODULE_AUTO_INIT_GNRC_UDP 1
#define MODULE_AUTO_INIT_RANDOM 1
#define MODULE_AUTO_INIT_SAUL 1
#define MODULE_AUTO_INIT_ZTIMER 1
#define MODULE_BOARD 1
#define MODULE_BOARD_COMMON_INIT 1
#define MODULE_BOARDS_COMMON_NRF52XXXDK 1
#define MODULE_CMSIS 1
#define MODULE_CORE 1
#define MODULE_CORE_INIT 1
#define MODULE_CORE_LIB 1
#define MODULE_CORE_MBOX 1
#define MODULE_CORE_MSG 1
#define MODULE_CORE_PANIC 1
#define MODULE_CORE_THREAD 1
#define MODULE_CORE_THREAD_FLAGS 1
#define MODULE_CORTEXM_COMMON 1
#define MODULE_CORTEXM_COMMON_PERIPH 1
#define MODULE_CORTEXM_FPU 1
#define MODULE_CPU 1
#define MODULE_CPU_COMMON 1
#define MODULE_DIV 1
#define MODULE_EUI_PROVIDER 1
#define MODULE_EVENT 1
#define MODULE_EVTIMER 1
#define MODULE_EVTIMER_MBOX 1
#define MODULE_FMT 1
#define MODULE_FRAC 1
#define MODULE_GNRC 1
#define MODULE_GNRC_ICMPV6 1
#define MODULE_GNRC_ICMPV6_ECHO 1
#define MODULE_GNRC_ICMPV6_ERROR 1
#define MODULE_GNRC_IPV6 1
#define MODULE_GNRC_IPV6_DEFAULT 1
#define MODULE_GNRC_IPV6_HDR 1
#define MODULE_GNRC_IPV6_NIB 1
#define MODULE_GNRC_IPV6_NIB_6LN 1
#define MODULE_GNRC_NDP 1
#define MODULE_GNRC_NETAPI 1
#define MODULE_GNRC_NETAPI_CALLBACKS 1
#define MODULE_GNRC_NETAPI_MBOX 1
#define MODULE_GNRC_NETIF 1
#define MODULE_GNRC_NETIF_6LO 1
#define MODULE_GNRC_NETIF_HDR 1
#define MODULE_GNRC_NETIF_IEEE802154 1
#define MODULE_GNRC_NETIF_INIT_DEVS 1
#define MODULE_GNRC_NETIF_IPV6 1
#define MODULE_GNRC_NETIF_PKTQ 1
#define MODULE_GNRC_NETREG 1
#define MODULE_GNRC_NETTYPE_ICMPV6 1
#define MODULE_GNRC_NETTYPE_IPV6 1
#define MODULE_GNRC_NETTYPE_SIXLOWPAN 1
#define MODULE_GNRC_NETTYPE_TCP 1
#define MODULE_GNRC_NETTYPE_UDP 1
#define MODULE_GNRC_PKT 1
#define MODULE_GNRC_PKTBUF 1
#define MODULE_GNRC_PKTBUF_STATIC 1
#define MODULE_GNRC_SIXLOWPAN 1
#define MODULE_GNRC_SIXLOWPAN_CTX 1
#define MODULE_GNRC_SIXLOWPAN_DEFAULT 1
#define MODULE_GNRC_SIXLOWPAN_FRAG 1
#define MODULE_GNRC_SIXLOWPAN_FRAG_FB 1
#define MODULE_GNRC_SIXLOWPAN_FRAG_RB 1
#define MODULE_GNRC_SIXLOWPAN_IPHC 1
#define MODULE_GNRC_SIXLOWPAN_IPHC_NHC 1
#define MODULE_GNRC_SIXLOWPAN_ND 1
#define MODULE_GNRC_SOCK 1
#define MODULE_GNRC_SOCK_ASYNC 1
#define MODULE_GNRC_SOCK_IP 1
#define MODULE_GNRC_SOCK_TCP 1
#define MODULE_GNRC_SOCK_UDP 1
#define MODULE_GNRC_TCP 1
#define MODULE_GNRC_UDP 1
#define MODULE_ICMPV6 1
#define MODULE_IEEE802154 1
#define MODULE_IEEE802154_SUBMAC 1
#define MODULE_INET_CSUM 1
#define MODULE_IOLIST 1
#define MODULE_IPV6 1
#define MODULE_IPV6_ADDR 1
#define MODULE_IPV6_HDR 1
#define MODULE_ISRPIPE 1
#define MODULE_L2UTIL 1
#define MODULE_LIBC 1
#define MODULE_LUID 1
#define MODULE_MALLOC_THREAD_SAFE 1
#define MODULE_MPU_STACK_GUARD 1
#define MODULE_NETDEV 1
#define MODULE_NETDEV_DEFAULT 1
#define MODULE_NETDEV_IEEE802154 1
#define MODULE_NETDEV_IEEE802154_SUBMAC 1
#define MODULE_NETDEV_LEGACY_API 1
#define MODULE_NETDEV_REGISTER 1
#define MODULE_NETIF 1
#define MODULE_NETUTILS 1
#define MODULE_NEWLIB 1
#define MODULE_NEWLIB_NANO 1
#define MODULE_NEWLIB_SYSCALLS_DEFAULT 1
#define MODULE_NRF52_VECTORS 1
#define MODULE_NRF5X_COMMON_PERIPH 1
#define MODULE_NRF802154 1
#define MODULE_PAHO_MQTT 1
#define MODULE_PAHO_MQTT_CONTRIB 1
#define MODULE_PAHO_MQTT_PACKET 1
#define MODULE_PERIPH 1
#define MODULE_PERIPH_COMMON 1
#define MODULE_PERIPH_CPUID 1
#define MODULE_PERIPH_GPIO 1
#define MODULE_PERIPH_GPIO_LL_DISCONNECT 1
#define MODULE_PERIPH_GPIO_LL_INPUT_PULL_DOWN 1
#define MODULE_PERIPH_GPIO_LL_INPUT_PULL_UP 1
#define MODULE_PERIPH_GPIO_LL_IRQ_EDGE_TRIGGERED_BOTH 1
#define MODULE_PERIPH_GPIO_LL_IRQ_UNMASK 1
#define MODULE_PERIPH_GPIO_LL_OPEN_DRAIN 1
#define MODULE_PERIPH_GPIO_LL_OPEN_DRAIN_PULL_UP 1
#define MODULE_PERIPH_GPIO_LL_OPEN_SOURCE 1
#define MODULE_PERIPH_GPIO_LL_OPEN_SOURCE_PULL_DOWN 1
#define MODULE_PERIPH_HWRNG 1
#define MODULE_PERIPH_INIT 1
#define MODULE_PERIPH_INIT_CPUID 1
#define MODULE_PERIPH_INIT_GPIO 1
#define MODULE_PERIPH_INIT_HWRNG 1
#define MODULE_PERIPH_INIT_LED0 1
#define MODULE_PERIPH_INIT_LED1 1
#define MODULE_PERIPH_INIT_LED2 1
#define MODULE_PERIPH_INIT_LED3 1
#define MODULE_PERIPH_INIT_LED4 1
#define MODULE_PERIPH_INIT_LED5 1
#define MODULE_PERIPH_INIT_LED6 1
#define MODULE_PERIPH_INIT_LED7 1
#define MODULE_PERIPH_INIT_LEDS 1
#define MODULE_PERIPH_INIT_PM 1
#define MODULE_PERIPH_INIT_PWM 1
#define MODULE_PERIPH_INIT_RTT 1
#define MODULE_PERIPH_INIT_TEMPERATURE 1
#define MODULE_PERIPH_INIT_TIMER 1
#define MODULE_PERIPH_INIT_UART 1
#define MODULE_PERIPH_PM 1
#define MODULE_PERIPH_PWM 1
#define MODULE_PERIPH_RTT 1
#define MODULE_PERIPH_TEMPERATURE 1
#define MODULE_PERIPH_TIMER 1
#define MODULE_PERIPH_UART 1
#define MODULE_PHYDAT 1
#define MODULE_PREPROCESSOR 1
#define MODULE_PREPROCESSOR_SUCCESSOR 1
#define MODULE_PRNG 1
#define MODULE_PRNG_MUSL_LCG 1
#define MODULE_PS 1
#define MODULE_RANDOM 1
#define MODULE_SAUL 1
#define MODULE_SAUL_DEFAULT 1
#define MODULE_SAUL_GPIO 1
#define MODULE_SAUL_INIT_DEVS 1
#define MODULE_SAUL_NRF_TEMPERATURE 1
#define MODULE_SAUL_PWM 1
#define MODULE_SAUL_REG 1
#define MODULE_SHELL 1
#define MODULE_SHELL_CMD_GNRC_ICMPV6_ECHO 1
#define MODULE_SHELL_CMD_GNRC_IPV6_NIB 1
#define MODULE_SHELL_CMD_GNRC_NETIF 1
#define MODULE_SHELL_CMD_GNRC_SIXLOWPAN_CTX 1
#define MODULE_SHELL_CMD_PM 1
#define MODULE_SHELL_CMD_PS 1
#define MODULE_SHELL_CMD_SAUL_REG 1
#define MODULE_SHELL_CMD_SYS 1
#define MODULE_SHELL_CMDS 1
#define MODULE_SHELL_CMDS_DEFAULT 1
#define MODULE_SIXLOWPAN 1
#define MODULE_SOCK 1
#define MODULE_SOCK_ASYNC 1
#define MODULE_SOCK_ASYNC_EVENT 1
#define MODULE_SOCK_IP 1
#define MODULE_SOCK_TCP 1
#define MODULE_SOCK_UDP 1
#define MODULE_STDIN 1
#define MODULE_STDIO 1
#define MODULE_STDIO_AVAILABLE 1
#define MODULE_STDIO_UART 1
#define MODULE_STDIO_UART_RX 1
#define MODULE_SYS 1
#define MODULE_TCP 1
#define MODULE_TSRB 1
#define MODULE_UDP 1
#define MODULE_VDD_LC_FILTER_REG0 1
#define MODULE_VDD_LC_FILTER_REG1 1
#define MODULE_XTIMER 1
#define MODULE_ZTIMER 1
#define MODULE_ZTIMER_CONVERT 1
#define MODULE_ZTIMER_CONVERT_FRAC 1
#define MODULE_ZTIMER_CONVERT_SHIFT 1
#define MODULE_ZTIMER_CORE 1
#define MODULE_ZTIMER_EXTEND 1
#define MODULE_ZTIMER_INIT 1
#define MODULE_ZTIMER_MSEC 1
#define MODULE_ZTIMER_PERIPH_RTT 1
#define MODULE_ZTIMER_PERIPH_TIMER 1
#define MODULE_ZTIMER_SEC 1
#define MODULE_ZTIMER_USEC 1
#define MODULE_ZTIMER_XTIMER_COMPAT 1
