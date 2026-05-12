# ----------------------------------------------------------------------------
#     _____
#    / #   /____   \____
#  / \===\   \==/
# /___\===\___\/  AVNET Design Resource Center
#      \======/         www.em.avnet.com/drc
#       \====/
# ----------------------------------------------------------------------------
#
#  Created With Avnet UCF Generator V0.4.0
#     Date: Saturday, June 30, 2012
#     Time: 12:18:55 AM
#
#  This design is the property of Avnet.  Publication of this
#  design is not authorized without written consent from Avnet.
#
#  Please direct any questions to:
#     ZedBoard.org Community Forums
#     http://www.zedboard.org
#
#  Disclaimer:
#     Avnet, Inc. makes no warranty for the use of this code or design.
#     This code is provided  "As Is". Avnet, Inc assumes no responsibility for
#     any errors, which may appear in this code, nor does it make a commitment
#     to update the information contained herein. Avnet, Inc specifically
#     disclaims any implied warranties of fitness for a particular purpose.
#                      Copyright(c) 2012 Avnet, Inc.
#                              All rights reserved.
#
# ----------------------------------------------------------------------------
#
#  Notes:
#
#  10 August 2012
#     IO standards based upon Bank 34 and Bank 35 Vcco supply options of 1.8V,
#     2.5V, or 3.3V are possible based upon the Vadj jumper (J18) settings.
#     By default, Vadj is expected to be set to 1.8V but if a different
#     voltage is used for a particular design, then the corresponding IO
#     standard within this UCF should also be updated to reflect the actual
#     Vadj jumper selection.
#
#  09 September 2012
#     Net names are not allowed to contain hyphen characters '-' since this
#     is not a legal VHDL87 or Verilog character within an identifier.
#     HDL net names are adjusted to contain no hyphen characters '-' but
#     rather use underscore '_' characters.  Comment net name with the hyphen
#     characters will remain in place since these are intended to match the
#     schematic net names in order to better enable schematic search.
#
#  17 April 2014
#     Pin constraint for toggle switch SW7 was corrected to M15 location.
#
#  16 April 2015
#     Corrected the way that entire banks are assigned to a particular IO
#     standard so that it works with more recent versions of Vivado Design
#     Suite and moved the IO standard constraints to the end of the file
#     along with some better organization and notes like we do with our SOMs.
#
#   6 June 2016
#     Corrected error in signal name for package pin N19 (FMC Expansion Connector)
#
#
# ----------------------------------------------------------------------------

# ----------------------------------------------------------------------------
# Audio Codec - Bank 13
# ----------------------------------------------------------------------------
#set_property PACKAGE_PIN AB1 [get_ports {AC_ADR0}];  # "AC-ADR0"
#set_property PACKAGE_PIN Y5  [get_ports {AC_ADR1}];  # "AC-ADR1"
#set_property PACKAGE_PIN Y8  [get_ports {SDATA_O}];  # "AC-GPIO0"
#set_property PACKAGE_PIN AA7 [get_ports {SDATA_I}];  # "AC-GPIO1"
#set_property PACKAGE_PIN AA6 [get_ports {BCLK_O}];  # "AC-GPIO2"
#set_property PACKAGE_PIN Y6  [get_ports {LRCLK_O}];  # "AC-GPIO3"
#set_property PACKAGE_PIN AB2 [get_ports {MCLK_O}];  # "AC-MCLK"
#set_property PACKAGE_PIN AB4 [get_ports {iic_rtl_scl_io}];  # "AC-SCK"
#set_property PACKAGE_PIN AB5 [get_ports {iic_rtl_sda_io}];  # "AC-SDA"

# ----------------------------------------------------------------------------
# Clock Source - Bank 13
# ----------------------------------------------------------------------------
#set_property PACKAGE_PIN Y9 [get_ports {GCLK}];  # "GCLK"

# ----------------------------------------------------------------------------
# JA Pmod - Bank 13
# ----------------------------------------------------------------------------
#set_property PACKAGE_PIN Y11  [get_ports {JA1}];  # "JA1"
#set_property PACKAGE_PIN AA11 [get_ports {JA2}];  # "JA2"
#set_property PACKAGE_PIN Y10  [get_ports {JA3}];  # "JA3"
#set_property PACKAGE_PIN AA9  [get_ports {JA4}];  # "JA4"
#set_property PACKAGE_PIN AB11 [get_ports {JA7}];  # "JA7"
#set_property PACKAGE_PIN AB10 [get_ports {JA8}];  # "JA8"
#set_property PACKAGE_PIN AB9  [get_ports {JA9}];  # "JA9"
#set_property PACKAGE_PIN AA8  [get_ports {JA10}];  # "JA10"


# ----------------------------------------------------------------------------
# JB Pmod - Bank 13
# ----------------------------------------------------------------------------
#set_property PACKAGE_PIN W12 [get_ports {JB1}];  # "JB1"
#set_property PACKAGE_PIN W11 [get_ports {JB2}];  # "JB2"
#set_property PACKAGE_PIN V10 [get_ports {JB3}];  # "JB3"
#set_property PACKAGE_PIN W8 [get_ports {JB4}];  # "JB4"
#set_property PACKAGE_PIN V12 [get_ports {JB7}];  # "JB7"
#set_property PACKAGE_PIN W10 [get_ports {JB8}];  # "JB8"
#set_property PACKAGE_PIN V9 [get_ports {JB9}];  # "JB9"
#set_property PACKAGE_PIN V8 [get_ports {JB10}];  # "JB10"

# ----------------------------------------------------------------------------
# JC Pmod - Bank 13
# ----------------------------------------------------------------------------
#set_property PACKAGE_PIN AB6 [get_ports {JC1_N}];  # "JC1_N"
#set_property PACKAGE_PIN AB7 [get_ports {JC1_P}];  # "JC1_P"
#set_property PACKAGE_PIN AA4 [get_ports {JC2_N}];  # "JC2_N"
#set_property PACKAGE_PIN Y4  [get_ports {JC2_P}];  # "JC2_P"
#set_property PACKAGE_PIN T6  [get_ports {JC3_N}];  # "JC3_N"
#set_property PACKAGE_PIN R6  [get_ports {JC3_P}];  # "JC3_P"
#set_property PACKAGE_PIN U4  [get_ports {JC4_N}];  # "JC4_N"
#set_property PACKAGE_PIN T4  [get_ports {JC4_P}];  # "JC4_P"

# ----------------------------------------------------------------------------
# JD Pmod - Bank 13
# ----------------------------------------------------------------------------
#set_property PACKAGE_PIN W7 [get_ports {JD1_N}];  # "JD1_N"
#set_property PACKAGE_PIN V7 [get_ports {JD1_P}];  # "JD1_P"
#set_property PACKAGE_PIN V4 [get_ports {JD2_N}];  # "JD2_N"
#set_property PACKAGE_PIN V5 [get_ports {JD2_P}];  # "JD2_P"
#set_property PACKAGE_PIN W5 [get_ports {JD3_N}];  # "JD3_N"
#set_property PACKAGE_PIN W6 [get_ports {JD3_P}];  # "JD3_P"
#set_property PACKAGE_PIN U5 [get_ports {JD4_N}];  # "JD4_N"
#set_property PACKAGE_PIN U6 [get_ports {JD4_P}];  # "JD4_P"

# ----------------------------------------------------------------------------
# OLED Display - Bank 13
# ----------------------------------------------------------------------------
#set_property PACKAGE_PIN U10  [get_ports {OLED_DC}];  # "OLED-DC"
#set_property PACKAGE_PIN U9   [get_ports {OLED_RES}];  # "OLED-RES"
#set_property PACKAGE_PIN AB12 [get_ports {OLED_SCLK}];  # "OLED-SCLK"
#set_property PACKAGE_PIN AA12 [get_ports {OLED_SDIN}];  # "OLED-SDIN"
#set_property PACKAGE_PIN U11  [get_ports {OLED_VBAT}];  # "OLED-VBAT"
#set_property PACKAGE_PIN U12  [get_ports {OLED_VDD}];  # "OLED-VDD"

# ----------------------------------------------------------------------------
# HDMI Output - Bank 33
# ----------------------------------------------------------------------------
#set_property PACKAGE_PIN W18  [get_ports {HD_CLK}];  # "HD-CLK"
#set_property PACKAGE_PIN Y13  [get_ports {HD_D0}];  # "HD-D0"
#set_property PACKAGE_PIN AA13 [get_ports {HD_D1}];  # "HD-D1"
#set_property PACKAGE_PIN W13  [get_ports {HD_D10}];  # "HD-D10"
#set_property PACKAGE_PIN W15  [get_ports {HD_D11}];  # "HD-D11"
#set_property PACKAGE_PIN V15  [get_ports {HD_D12}];  # "HD-D12"
#set_property PACKAGE_PIN U17  [get_ports {HD_D13}];  # "HD-D13"
#set_property PACKAGE_PIN V14  [get_ports {HD_D14}];  # "HD-D14"
#set_property PACKAGE_PIN V13  [get_ports {HS_D15}];  # "HD-D15"
#set_property PACKAGE_PIN AA14 [get_ports {HD_D2}];  # "HD-D2"
#set_property PACKAGE_PIN Y14  [get_ports {HD_D3}];  # "HD-D3"
#set_property PACKAGE_PIN AB15 [get_ports {HD_D4}];  # "HD-D4"
#set_property PACKAGE_PIN AB16 [get_ports {HD_D5}];  # "HD-D5"
#set_property PACKAGE_PIN AA16 [get_ports {HD_D6}];  # "HD-D6"
#set_property PACKAGE_PIN AB17 [get_ports {HD_D7}];  # "HD-D7"
#set_property PACKAGE_PIN AA17 [get_ports {HD_D8}];  # "HD-D8"
#set_property PACKAGE_PIN Y15  [get_ports {HD_D9}];  # "HD-D9"
#set_property PACKAGE_PIN U16  [get_ports {HD_DE}];  # "HD-DE"
#set_property PACKAGE_PIN V17  [get_ports {HD_HSYNC}];  # "HD-HSYNC"
#set_property PACKAGE_PIN W16  [get_ports {HD_INT}];  # "HD-INT"
#set_property PACKAGE_PIN AA18 [get_ports {HD_SCL}];  # "HD-SCL"
#set_property PACKAGE_PIN Y16  [get_ports {HD_SDA}];  # "HD-SDA"
#set_property PACKAGE_PIN U15  [get_ports {HD_SPDIF}];  # "HD-SPDIF"
#set_property PACKAGE_PIN Y18  [get_ports {HD_SPDIFO}];  # "HD-SPDIFO"
#set_property PACKAGE_PIN W17  [get_ports {HD_VSYNC}];  # "HD-VSYNC"

# ----------------------------------------------------------------------------
# User LEDs - Bank 33
# ----------------------------------------------------------------------------
set_property PACKAGE_PIN T22 [get_ports {leds_tri_o[0]}]
set_property PACKAGE_PIN T21 [get_ports {leds_tri_o[1]}]
#
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_0/s_axi_araddr[8]}]
set_property MARK_DEBUG true [get_nets design_1_i/axi_gpio_0/s_axi_aresetn]
set_property MARK_DEBUG true [get_nets design_1_i/axi_gpio_0/s_axi_bvalid]
set_property MARK_DEBUG true [get_nets design_1_i/axi_gpio_0/s_axi_arready]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_0/gpio_io_o[7]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_0/s_axi_awaddr[3]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_0/s_axi_rdata[1]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_0/s_axi_wdata[5]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_0/s_axi_wdata[6]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_0/gpio_io_o[1]}]
set_property MARK_DEBUG true [get_nets design_1_i/axi_gpio_0/s_axi_awready]
set_property MARK_DEBUG true [get_nets design_1_i/axi_gpio_0/s_axi_aclk]
set_property MARK_DEBUG true [get_nets design_1_i/axi_gpio_0/s_axi_rvalid]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_0/gpio_io_o[3]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_0/s_axi_rdata[7]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_0/s_axi_wdata[0]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_0/s_axi_wdata[1]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_0/s_axi_rdata[4]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_0/s_axi_rdata[2]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_0/gpio_io_o[4]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_0/gpio_io_o[5]}]
set_property MARK_DEBUG true [get_nets design_1_i/axi_gpio_0/s_axi_bready]
set_property MARK_DEBUG true [get_nets design_1_i/axi_gpio_0/s_axi_wvalid]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_0/s_axi_araddr[3]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_0/s_axi_rdata[0]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_0/s_axi_wdata[3]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_0/s_axi_rdata[6]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_0/gpio_io_o[0]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_0/s_axi_awaddr[2]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_0/s_axi_wdata[7]}]
set_property MARK_DEBUG true [get_nets design_1_i/axi_gpio_0/s_axi_arvalid]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_0/s_axi_awaddr[8]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_0/s_axi_rdata[3]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_0/s_axi_rdata[5]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_0/gpio_io_o[6]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_0/gpio_io_o[2]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_0/s_axi_araddr[2]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_0/s_axi_wdata[2]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_0/s_axi_wdata[4]}]
set_property MARK_DEBUG true [get_nets design_1_i/axi_gpio_0/s_axi_awvalid]
set_property MARK_DEBUG true [get_nets design_1_i/axi_gpio_0/s_axi_rready]
set_property MARK_DEBUG true [get_nets design_1_i/axi_gpio_0/s_axi_wready]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_1/s_axi_rdata[1]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_1/s_axi_wdata[2]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_1/gpio_io_i[6]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_1/s_axi_awaddr[8]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_1/s_axi_rdata[3]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_1/s_axi_wdata[7]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_1/gpio_io_i[4]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_1/s_axi_araddr[2]}]
set_property MARK_DEBUG true [get_nets design_1_i/axi_gpio_1/s_axi_arvalid]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_1/s_axi_rdata[4]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_1/gpio_io_i[0]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_1/s_axi_awaddr[2]}]
set_property MARK_DEBUG true [get_nets design_1_i/axi_gpio_1/s_axi_aresetn]
set_property MARK_DEBUG true [get_nets design_1_i/axi_gpio_1/s_axi_arready]
set_property MARK_DEBUG true [get_nets design_1_i/axi_gpio_1/s_axi_awready]
set_property MARK_DEBUG true [get_nets design_1_i/axi_gpio_1/s_axi_awvalid]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_1/s_axi_rdata[7]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_1/s_axi_wdata[0]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_1/s_axi_wdata[1]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_1/s_axi_wdata[4]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_1/gpio_io_i[1]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_1/gpio_io_i[2]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_1/gpio_io_i[5]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_1/s_axi_araddr[8]}]
set_property MARK_DEBUG true [get_nets design_1_i/axi_gpio_1/s_axi_aclk]
set_property MARK_DEBUG true [get_nets design_1_i/axi_gpio_1/s_axi_rvalid]
set_property MARK_DEBUG true [get_nets design_1_i/axi_gpio_1/s_axi_wvalid]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_1/s_axi_rdata[5]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_1/s_axi_wdata[6]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_1/s_axi_awaddr[3]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_1/s_axi_araddr[3]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_1/s_axi_rdata[0]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_1/s_axi_rdata[6]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_1/s_axi_wdata[5]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_1/gpio_io_i[3]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_1/gpio_io_i[7]}]
set_property MARK_DEBUG true [get_nets design_1_i/axi_gpio_1/s_axi_wready]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_1/s_axi_rdata[2]}]
set_property MARK_DEBUG true [get_nets {design_1_i/axi_gpio_1/s_axi_wdata[3]}]
set_property MARK_DEBUG true [get_nets design_1_i/axi_gpio_1/s_axi_bready]
set_property MARK_DEBUG true [get_nets design_1_i/axi_gpio_1/s_axi_bvalid]
set_property MARK_DEBUG true [get_nets design_1_i/axi_gpio_1/s_axi_rready]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_1/s_axi_rdata[12]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_1/s_axi_rdata[26]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_0/s_axi_rdata[12]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_0/s_axi_rdata[19]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_0/s_axi_rdata[25]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_0/s_axi_rdata[29]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_0/s_axi_wdata[26]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_0/s_axi_wdata[27]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_1/s_axi_wdata[28]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_1/s_axi_wdata[29]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_1/s_axi_rdata[10]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_1/s_axi_rdata[24]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_0/s_axi_rdata[10]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_0/s_axi_rdata[13]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_0/s_axi_rdata[21]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_1/s_axi_rdata[22]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_1/s_axi_rdata[28]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_1/s_axi_wdata[30]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_1/s_axi_rdata[16]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_1/s_axi_rdata[27]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_1/s_axi_rdata[17]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_0/s_axi_rdata[18]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_0/s_axi_rdata[22]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_1/s_axi_rdata[30]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_0/s_axi_wdata[24]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_0/s_axi_wdata[28]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_1/s_axi_rdata[25]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_0/s_axi_rdata[8]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_0/s_axi_rdata[17]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_0/s_axi_rdata[20]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_0/s_axi_rdata[28]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_0/s_axi_rdata[30]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_1/s_axi_wdata[24]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_0/s_axi_wdata[31]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_1/s_axi_rdata[9]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_1/s_axi_rdata[23]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_0/s_axi_rdata[27]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_1/s_axi_rdata[31]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_1/s_axi_wdata[27]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_1/s_axi_rdata[14]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_1/s_axi_wdata[31]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_0/s_axi_rdata[23]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_0/s_axi_rdata[31]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_0/s_axi_rdata[26]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_1/s_axi_rdata[29]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_0/s_axi_wdata[29]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_1/s_axi_rdata[8]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_0/s_axi_rdata[15]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_0/s_axi_rdata[9]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_0/s_axi_rdata[14]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_0/s_axi_rdata[16]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_1/s_axi_rdata[20]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_1/s_axi_rdata[21]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_1/s_axi_wdata[25]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_1/s_axi_wdata[26]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_0/s_axi_wdata[30]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_1/s_axi_rdata[11]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_1/s_axi_rdata[13]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_1/s_axi_rdata[15]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_1/s_axi_rdata[18]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_0/s_axi_rdata[11]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_1/s_axi_rdata[19]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_0/s_axi_rdata[24]}]
set_property MARK_DEBUG false [get_nets {design_1_i/axi_gpio_0/s_axi_wdata[25]}]
create_debug_core u_ila_0 ila
set_property ALL_PROBE_SAME_MU true [get_debug_cores u_ila_0]
set_property ALL_PROBE_SAME_MU_CNT 2 [get_debug_cores u_ila_0]
set_property C_ADV_TRIGGER false [get_debug_cores u_ila_0]
set_property C_DATA_DEPTH 2048 [get_debug_cores u_ila_0]
set_property C_EN_STRG_QUAL true [get_debug_cores u_ila_0]
set_property C_INPUT_PIPE_STAGES 0 [get_debug_cores u_ila_0]
set_property C_TRIGIN_EN false [get_debug_cores u_ila_0]
set_property C_TRIGOUT_EN false [get_debug_cores u_ila_0]
set_property port_width 1 [get_debug_ports u_ila_0/clk]
connect_debug_port u_ila_0/clk [get_nets [list design_1_i/processing_system7_0/inst/FCLK_CLK0]]
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe0]
set_property port_width 8 [get_debug_ports u_ila_0/probe0]
connect_debug_port u_ila_0/probe0 [get_nets [list {design_1_i/axi_gpio_0/gpio_io_o[0]} {design_1_i/axi_gpio_0/gpio_io_o[1]} {design_1_i/axi_gpio_0/gpio_io_o[2]} {design_1_i/axi_gpio_0/gpio_io_o[3]} {design_1_i/axi_gpio_0/gpio_io_o[4]} {design_1_i/axi_gpio_0/gpio_io_o[5]} {design_1_i/axi_gpio_0/gpio_io_o[6]} {design_1_i/axi_gpio_0/gpio_io_o[7]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe1]
set_property port_width 3 [get_debug_ports u_ila_0/probe1]
connect_debug_port u_ila_0/probe1 [get_nets [list {design_1_i/axi_gpio_0/s_axi_awaddr[2]} {design_1_i/axi_gpio_0/s_axi_awaddr[3]} {design_1_i/axi_gpio_0/s_axi_awaddr[8]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe2]
set_property port_width 8 [get_debug_ports u_ila_0/probe2]
connect_debug_port u_ila_0/probe2 [get_nets [list {design_1_i/axi_gpio_0/s_axi_rdata[0]} {design_1_i/axi_gpio_0/s_axi_rdata[1]} {design_1_i/axi_gpio_0/s_axi_rdata[2]} {design_1_i/axi_gpio_0/s_axi_rdata[3]} {design_1_i/axi_gpio_0/s_axi_rdata[4]} {design_1_i/axi_gpio_0/s_axi_rdata[5]} {design_1_i/axi_gpio_0/s_axi_rdata[6]} {design_1_i/axi_gpio_0/s_axi_rdata[7]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe3]
set_property port_width 8 [get_debug_ports u_ila_0/probe3]
connect_debug_port u_ila_0/probe3 [get_nets [list {design_1_i/axi_gpio_0/s_axi_wdata[0]} {design_1_i/axi_gpio_0/s_axi_wdata[1]} {design_1_i/axi_gpio_0/s_axi_wdata[2]} {design_1_i/axi_gpio_0/s_axi_wdata[3]} {design_1_i/axi_gpio_0/s_axi_wdata[4]} {design_1_i/axi_gpio_0/s_axi_wdata[5]} {design_1_i/axi_gpio_0/s_axi_wdata[6]} {design_1_i/axi_gpio_0/s_axi_wdata[7]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe4]
set_property port_width 3 [get_debug_ports u_ila_0/probe4]
connect_debug_port u_ila_0/probe4 [get_nets [list {design_1_i/axi_gpio_0/s_axi_araddr[2]} {design_1_i/axi_gpio_0/s_axi_araddr[3]} {design_1_i/axi_gpio_0/s_axi_araddr[8]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe5]
set_property port_width 3 [get_debug_ports u_ila_0/probe5]
connect_debug_port u_ila_0/probe5 [get_nets [list {design_1_i/axi_gpio_1/s_axi_araddr[2]} {design_1_i/axi_gpio_1/s_axi_araddr[3]} {design_1_i/axi_gpio_1/s_axi_araddr[8]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe6]
set_property port_width 8 [get_debug_ports u_ila_0/probe6]
connect_debug_port u_ila_0/probe6 [get_nets [list {design_1_i/axi_gpio_1/s_axi_wdata[0]} {design_1_i/axi_gpio_1/s_axi_wdata[1]} {design_1_i/axi_gpio_1/s_axi_wdata[2]} {design_1_i/axi_gpio_1/s_axi_wdata[3]} {design_1_i/axi_gpio_1/s_axi_wdata[4]} {design_1_i/axi_gpio_1/s_axi_wdata[5]} {design_1_i/axi_gpio_1/s_axi_wdata[6]} {design_1_i/axi_gpio_1/s_axi_wdata[7]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe7]
set_property port_width 8 [get_debug_ports u_ila_0/probe7]
connect_debug_port u_ila_0/probe7 [get_nets [list {design_1_i/axi_gpio_1/gpio_io_i[0]} {design_1_i/axi_gpio_1/gpio_io_i[1]} {design_1_i/axi_gpio_1/gpio_io_i[2]} {design_1_i/axi_gpio_1/gpio_io_i[3]} {design_1_i/axi_gpio_1/gpio_io_i[4]} {design_1_i/axi_gpio_1/gpio_io_i[5]} {design_1_i/axi_gpio_1/gpio_io_i[6]} {design_1_i/axi_gpio_1/gpio_io_i[7]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe8]
set_property port_width 3 [get_debug_ports u_ila_0/probe8]
connect_debug_port u_ila_0/probe8 [get_nets [list {design_1_i/axi_gpio_1/s_axi_awaddr[2]} {design_1_i/axi_gpio_1/s_axi_awaddr[3]} {design_1_i/axi_gpio_1/s_axi_awaddr[8]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe9]
set_property port_width 8 [get_debug_ports u_ila_0/probe9]
connect_debug_port u_ila_0/probe9 [get_nets [list {design_1_i/axi_gpio_1/s_axi_rdata[0]} {design_1_i/axi_gpio_1/s_axi_rdata[1]} {design_1_i/axi_gpio_1/s_axi_rdata[2]} {design_1_i/axi_gpio_1/s_axi_rdata[3]} {design_1_i/axi_gpio_1/s_axi_rdata[4]} {design_1_i/axi_gpio_1/s_axi_rdata[5]} {design_1_i/axi_gpio_1/s_axi_rdata[6]} {design_1_i/axi_gpio_1/s_axi_rdata[7]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe10]
set_property port_width 1 [get_debug_ports u_ila_0/probe10]
connect_debug_port u_ila_0/probe10 [get_nets [list design_1_i/axi_gpio_0/s_axi_aresetn]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe11]
set_property port_width 1 [get_debug_ports u_ila_0/probe11]
connect_debug_port u_ila_0/probe11 [get_nets [list design_1_i/axi_gpio_1/s_axi_aresetn]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe12]
set_property port_width 1 [get_debug_ports u_ila_0/probe12]
connect_debug_port u_ila_0/probe12 [get_nets [list design_1_i/axi_gpio_0/s_axi_arready]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe13]
set_property port_width 1 [get_debug_ports u_ila_0/probe13]
connect_debug_port u_ila_0/probe13 [get_nets [list design_1_i/axi_gpio_1/s_axi_arready]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe14]
set_property port_width 1 [get_debug_ports u_ila_0/probe14]
connect_debug_port u_ila_0/probe14 [get_nets [list design_1_i/axi_gpio_0/s_axi_arvalid]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe15]
set_property port_width 1 [get_debug_ports u_ila_0/probe15]
connect_debug_port u_ila_0/probe15 [get_nets [list design_1_i/axi_gpio_1/s_axi_arvalid]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe16]
set_property port_width 1 [get_debug_ports u_ila_0/probe16]
connect_debug_port u_ila_0/probe16 [get_nets [list design_1_i/axi_gpio_0/s_axi_awready]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe17]
set_property port_width 1 [get_debug_ports u_ila_0/probe17]
connect_debug_port u_ila_0/probe17 [get_nets [list design_1_i/axi_gpio_1/s_axi_awready]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe18]
set_property port_width 1 [get_debug_ports u_ila_0/probe18]
connect_debug_port u_ila_0/probe18 [get_nets [list design_1_i/axi_gpio_1/s_axi_awvalid]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe19]
set_property port_width 1 [get_debug_ports u_ila_0/probe19]
connect_debug_port u_ila_0/probe19 [get_nets [list design_1_i/axi_gpio_0/s_axi_awvalid]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe20]
set_property port_width 1 [get_debug_ports u_ila_0/probe20]
connect_debug_port u_ila_0/probe20 [get_nets [list design_1_i/axi_gpio_0/s_axi_bready]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe21]
set_property port_width 1 [get_debug_ports u_ila_0/probe21]
connect_debug_port u_ila_0/probe21 [get_nets [list design_1_i/axi_gpio_1/s_axi_bready]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe22]
set_property port_width 1 [get_debug_ports u_ila_0/probe22]
connect_debug_port u_ila_0/probe22 [get_nets [list design_1_i/axi_gpio_0/s_axi_bvalid]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe23]
set_property port_width 1 [get_debug_ports u_ila_0/probe23]
connect_debug_port u_ila_0/probe23 [get_nets [list design_1_i/axi_gpio_1/s_axi_bvalid]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe24]
set_property port_width 1 [get_debug_ports u_ila_0/probe24]
connect_debug_port u_ila_0/probe24 [get_nets [list design_1_i/axi_gpio_0/s_axi_rready]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe25]
set_property port_width 1 [get_debug_ports u_ila_0/probe25]
connect_debug_port u_ila_0/probe25 [get_nets [list design_1_i/axi_gpio_1/s_axi_rready]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe26]
set_property port_width 1 [get_debug_ports u_ila_0/probe26]
connect_debug_port u_ila_0/probe26 [get_nets [list design_1_i/axi_gpio_1/s_axi_rvalid]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe27]
set_property port_width 1 [get_debug_ports u_ila_0/probe27]
connect_debug_port u_ila_0/probe27 [get_nets [list design_1_i/axi_gpio_0/s_axi_rvalid]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe28]
set_property port_width 1 [get_debug_ports u_ila_0/probe28]
connect_debug_port u_ila_0/probe28 [get_nets [list design_1_i/axi_gpio_1/s_axi_wready]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe29]
set_property port_width 1 [get_debug_ports u_ila_0/probe29]
connect_debug_port u_ila_0/probe29 [get_nets [list design_1_i/axi_gpio_0/s_axi_wready]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe30]
set_property port_width 1 [get_debug_ports u_ila_0/probe30]
connect_debug_port u_ila_0/probe30 [get_nets [list design_1_i/axi_gpio_1/s_axi_wvalid]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe31]
set_property port_width 1 [get_debug_ports u_ila_0/probe31]
connect_debug_port u_ila_0/probe31 [get_nets [list design_1_i/axi_gpio_0/s_axi_wvalid]]
set_property C_CLK_INPUT_FREQ_HZ 300000000 [get_debug_cores dbg_hub]
set_property C_ENABLE_CLK_DIVIDER false [get_debug_cores dbg_hub]
set_property C_USER_SCAN_CHAIN 1 [get_debug_cores dbg_hub]
connect_debug_port dbg_hub/clk [get_nets u_ila_0_FCLK_CLK0]
