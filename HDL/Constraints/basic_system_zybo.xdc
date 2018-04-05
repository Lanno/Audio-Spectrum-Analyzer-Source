## This file is a general .xdc for the ZYBO Rev B board
## To use it in a project:
## - uncomment the lines corresponding to used pins
## - rename the used signals according to the project


##Clock signal
#set_property -dict { PACKAGE_PIN L16   IOSTANDARD LVCMOS33 } [get_ports { clk }]; #IO_L11P_T1_SRCC_35 Sch=sysclk
#create_clock -add -name sys_clk_pin -period 8.00 -waveform {0 4} [get_ports { clk }];


##Switches
#set_property -dict { PACKAGE_PIN G15   IOSTANDARD LVCMOS33 } [get_ports { sw[0] }]; #IO_L19N_T3_VREF_35 Sch=SW0
#set_property -dict { PACKAGE_PIN P15   IOSTANDARD LVCMOS33 } [get_ports { sw[1] }];  #IO_L24P_T3_34 Sch=SW1
#set_property -dict { PACKAGE_PIN W13   IOSTANDARD LVCMOS33 } [get_ports { sw[2] }]; #IO_L4N_T0_34 Sch=SW2
#set_property -dict { PACKAGE_PIN T16   IOSTANDARD LVCMOS33 } [get_ports { sw[3] }]; #IO_L9P_T1_DQS_34 Sch=SW3


##Buttons
#set_property -dict { PACKAGE_PIN R18   IOSTANDARD LVCMOS33 } [get_ports { btn[0] }]; #IO_L20N_T3_34 Sch=BTN0
#set_property -dict { PACKAGE_PIN P16   IOSTANDARD LVCMOS33 } [get_ports { btn[1] }]; #IO_L24N_T3_34 Sch=BTN1
#set_property -dict { PACKAGE_PIN V16   IOSTANDARD LVCMOS33 } [get_ports { btn[2] }]; #IO_L18P_T2_34 Sch=BTN2
#set_property -dict { PACKAGE_PIN Y16   IOSTANDARD LVCMOS33 } [get_ports { btn[3] }]; #IO_L7P_T1_34 Sch=BTN3


##LEDs
#set_property -dict { PACKAGE_PIN M14   IOSTANDARD LVCMOS33 } [get_ports { led[0] }]; #IO_L23P_T3_35 Sch=LED0
#set_property -dict { PACKAGE_PIN M15   IOSTANDARD LVCMOS33 } [get_ports { led[1] }]; #IO_L23N_T3_35 Sch=LED1
#set_property -dict { PACKAGE_PIN G14   IOSTANDARD LVCMOS33 } [get_ports { led[2] }]; #IO_0_3S5=Sch=LED2
#set_property -dict { PACKAGE_PIN D18   IOSTANDARD LVCMOS33 } [get_ports { led[3] }]; #IO_L3N_T0_DQS_AD1N_35 Sch=LED3


##I2S Audio Codec
set_property -dict { PACKAGE_PIN K18   IOSTANDARD LVCMOS33 } [get_ports i2s_0_bclk_out ]; #IO_L12N_T1_MRCC_35 Sch=AC_BCLK
set_property -dict { PACKAGE_PIN T19   IOSTANDARD LVCMOS33 } [get_ports tx_clk_0       ]; #IO_25_34 Sch=AC_MCLK
set_property -dict { PACKAGE_PIN P18   IOSTANDARD LVCMOS33 } [get_ports {mute_tri_o[0]}]; #IO_L23N_T3_34 Sch=AC_MUTEN
set_property -dict { PACKAGE_PIN M17   IOSTANDARD LVCMOS33 } [get_ports i2s_0_s_out    ]; #IO_L8P_T1_AD10P_35 Sch=AC_PBDAT
set_property -dict { PACKAGE_PIN L17   IOSTANDARD LVCMOS33 } [get_ports i2s_0_ws_out   ]; #IO_L11N_T1_SRCC_35 Sch=AC_PBLRC
set_property -dict { PACKAGE_PIN K17   IOSTANDARD LVCMOS33 } [get_ports i2s_1_s_in     ]; #IO_L12P_T1_MRCC_35 Sch=AC_RECDAT
set_property -dict { PACKAGE_PIN M18   IOSTANDARD LVCMOS33 } [get_ports i2s_1_ws_out   ]; #IO_L8N_T1_AD10N_35 Sch=AC_RECLRC


##Audio Codec/external EEPROM IIC bus
set_property -dict { PACKAGE_PIN N18   IOSTANDARD LVCMOS33 } [get_ports iic_0_scl_io]; #IO_L13P_T2_MRCC_34 Sch=AC_SCL
set_property -dict { PACKAGE_PIN N17   IOSTANDARD LVCMOS33 } [get_ports iic_0_sda_io]; #IO_L23P_T3_34 Sch=AC_SDA
