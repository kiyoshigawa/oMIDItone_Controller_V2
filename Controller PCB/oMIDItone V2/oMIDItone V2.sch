EESchema Schematic File Version 4
LIBS:oMIDItone V2-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L teensy:Teensy3.2 U1
U 1 1 5CD70DA2
P 1850 2150
F 0 "U1" H 1850 3737 60  0000 C CNN
F 1 "Teensy3.2" H 1850 3631 60  0000 C CNN
F 2 "teensy:Teensy30_31_32_LC" H 1850 1350 60  0001 C CNN
F 3 "" H 1850 1350 60  0000 C CNN
	1    1850 2150
	1    0    0    -1  
$EndComp
NoConn ~ 2850 850 
NoConn ~ 2850 950 
NoConn ~ 2850 1050
NoConn ~ 850  2250
NoConn ~ 850  2550
$Comp
L power:GND #PWR023
U 1 1 5CD70F4B
P 2850 2350
F 0 "#PWR023" H 2850 2100 50  0001 C CNN
F 1 "GND" V 2855 2222 50  0000 R CNN
F 2 "" H 2850 2350 50  0001 C CNN
F 3 "" H 2850 2350 50  0001 C CNN
	1    2850 2350
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR025
U 1 1 5CD70F72
P 2850 3050
F 0 "#PWR025" H 2850 2800 50  0001 C CNN
F 1 "GND" V 2855 2922 50  0000 R CNN
F 2 "" H 2850 3050 50  0001 C CNN
F 3 "" H 2850 3050 50  0001 C CNN
	1    2850 3050
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR01
U 1 1 5CD70F99
P 850 850
F 0 "#PWR01" H 850 600 50  0001 C CNN
F 1 "GND" V 855 722 50  0000 R CNN
F 2 "" H 850 850 50  0001 C CNN
F 3 "" H 850 850 50  0001 C CNN
	1    850  850 
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR03
U 1 1 5CD70FBE
P 850 2450
F 0 "#PWR03" H 850 2200 50  0001 C CNN
F 1 "GND" V 855 2322 50  0000 R CNN
F 2 "" H 850 2450 50  0001 C CNN
F 3 "" H 850 2450 50  0001 C CNN
	1    850  2450
	0    1    1    0   
$EndComp
Wire Wire Line
	2850 2850 2850 2950
$Comp
L power:+5V #PWR024
U 1 1 5CD70FED
P 2850 2850
F 0 "#PWR024" H 2850 2700 50  0001 C CNN
F 1 "+5V" V 2865 2978 50  0000 L CNN
F 2 "" H 2850 2850 50  0001 C CNN
F 3 "" H 2850 2850 50  0001 C CNN
	1    2850 2850
	0    1    1    0   
$EndComp
Connection ~ 2850 2850
NoConn ~ 2850 2750
Text GLabel 850  2750 0    50   Input ~ 0
SCK
Text GLabel 850  2050 0    50   Input ~ 0
MOSI
$Comp
L kiyoshigawa_lib:MCP-4151 U8
U 1 1 5CD6E378
P 5550 1000
F 0 "U8" H 5550 1375 50  0000 C CNN
F 1 "MCP-4151" H 5550 1284 50  0000 C CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 5550 1350 50  0001 C CNN
F 3 "" H 5550 1350 50  0001 C CNN
	1    5550 1000
	1    0    0    -1  
$EndComp
$Comp
L kiyoshigawa_lib:MCP-4151 U14
U 1 1 5CD6E439
P 7150 1000
F 0 "U14" H 7150 1375 50  0000 C CNN
F 1 "MCP-4151" H 7150 1284 50  0000 C CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 7150 1350 50  0001 C CNN
F 3 "" H 7150 1350 50  0001 C CNN
	1    7150 1000
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR050
U 1 1 5CD6E44B
P 6050 850
F 0 "#PWR050" H 6050 700 50  0001 C CNN
F 1 "+5V" V 6065 978 50  0000 L CNN
F 2 "" H 6050 850 50  0001 C CNN
F 3 "" H 6050 850 50  0001 C CNN
	1    6050 850 
	0    1    1    0   
$EndComp
$Comp
L power:+5V #PWR065
U 1 1 5CD6E454
P 7650 850
F 0 "#PWR065" H 7650 700 50  0001 C CNN
F 1 "+5V" V 7665 978 50  0000 L CNN
F 2 "" H 7650 850 50  0001 C CNN
F 3 "" H 7650 850 50  0001 C CNN
	1    7650 850 
	0    1    1    0   
$EndComp
NoConn ~ 7650 950 
NoConn ~ 6050 950 
$Comp
L power:GND #PWR041
U 1 1 5CD6E46F
P 5050 1150
F 0 "#PWR041" H 5050 900 50  0001 C CNN
F 1 "GND" V 5055 1022 50  0000 R CNN
F 2 "" H 5050 1150 50  0001 C CNN
F 3 "" H 5050 1150 50  0001 C CNN
	1    5050 1150
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR058
U 1 1 5CD6E4A8
P 6650 1150
F 0 "#PWR058" H 6650 900 50  0001 C CNN
F 1 "GND" V 6655 1022 50  0000 R CNN
F 2 "" H 6650 1150 50  0001 C CNN
F 3 "" H 6650 1150 50  0001 C CNN
	1    6650 1150
	0    1    1    0   
$EndComp
Text GLabel 5050 1050 0    50   Input ~ 0
MOSI
Text GLabel 6650 1050 0    50   Input ~ 0
MOSI
Text GLabel 5050 950  0    50   Input ~ 0
SCK
Wire Wire Line
	6050 1050 6300 1050
Wire Wire Line
	6300 1050 6300 1250
Wire Wire Line
	6300 1250 7650 1250
Wire Wire Line
	7650 1250 7650 1150
$Comp
L kiyoshigawa_lib:otamatone OM1
U 1 1 5CD6F005
P 8500 1050
F 0 "OM1" H 8828 1101 50  0000 L CNN
F 1 "otamatone" H 8828 1010 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x05_P2.54mm_Vertical" H 8650 1350 50  0001 C CNN
F 3 "" H 8650 1350 50  0001 C CNN
	1    8500 1050
	1    0    0    -1  
$EndComp
Wire Wire Line
	7650 1050 7750 1050
Wire Wire Line
	7750 1050 7750 950 
Wire Wire Line
	7750 950  8400 950 
$Comp
L power:GND #PWR072
U 1 1 5CD6F382
P 8400 1250
F 0 "#PWR072" H 8400 1000 50  0001 C CNN
F 1 "GND" V 8405 1122 50  0000 R CNN
F 2 "" H 8400 1250 50  0001 C CNN
F 3 "" H 8400 1250 50  0001 C CNN
	1    8400 1250
	0    1    1    0   
$EndComp
$Comp
L power:+VDC #PWR071
U 1 1 5CD6F4C2
P 8400 850
F 0 "#PWR071" H 8400 750 50  0001 C CNN
F 1 "+VDC" V 8400 1079 50  0000 L CNN
F 2 "" H 8400 850 50  0001 C CNN
F 3 "" H 8400 850 50  0001 C CNN
	1    8400 850 
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR032
U 1 1 5CD6F835
P 3800 1500
F 0 "#PWR032" H 3800 1250 50  0001 C CNN
F 1 "GND" V 3805 1372 50  0000 R CNN
F 2 "" H 3800 1500 50  0001 C CNN
F 3 "" H 3800 1500 50  0001 C CNN
	1    3800 1500
	0    1    1    0   
$EndComp
Wire Wire Line
	6050 1250 6050 1150
Wire Wire Line
	4550 1300 7800 1300
Wire Wire Line
	7800 1300 7800 1050
Wire Wire Line
	7800 1050 8400 1050
Text GLabel 8400 1150 0    50   Input ~ 0
A10
$Comp
L kiyoshigawa_lib:MCP-4151 U9
U 1 1 5CD71331
P 5550 1950
F 0 "U9" H 5550 2325 50  0000 C CNN
F 1 "MCP-4151" H 5550 2234 50  0000 C CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 5550 2300 50  0001 C CNN
F 3 "" H 5550 2300 50  0001 C CNN
	1    5550 1950
	1    0    0    -1  
$EndComp
$Comp
L kiyoshigawa_lib:MCP-4151 U15
U 1 1 5CD71338
P 7150 1950
F 0 "U15" H 7150 2325 50  0000 C CNN
F 1 "MCP-4151" H 7150 2234 50  0000 C CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 7150 2300 50  0001 C CNN
F 3 "" H 7150 2300 50  0001 C CNN
	1    7150 1950
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR051
U 1 1 5CD7133F
P 6050 1800
F 0 "#PWR051" H 6050 1650 50  0001 C CNN
F 1 "+5V" V 6065 1928 50  0000 L CNN
F 2 "" H 6050 1800 50  0001 C CNN
F 3 "" H 6050 1800 50  0001 C CNN
	1    6050 1800
	0    1    1    0   
$EndComp
$Comp
L power:+5V #PWR066
U 1 1 5CD71345
P 7650 1800
F 0 "#PWR066" H 7650 1650 50  0001 C CNN
F 1 "+5V" V 7665 1928 50  0000 L CNN
F 2 "" H 7650 1800 50  0001 C CNN
F 3 "" H 7650 1800 50  0001 C CNN
	1    7650 1800
	0    1    1    0   
$EndComp
NoConn ~ 7650 1900
NoConn ~ 6050 1900
$Comp
L power:GND #PWR042
U 1 1 5CD7134D
P 5050 2100
F 0 "#PWR042" H 5050 1850 50  0001 C CNN
F 1 "GND" V 5055 1972 50  0000 R CNN
F 2 "" H 5050 2100 50  0001 C CNN
F 3 "" H 5050 2100 50  0001 C CNN
	1    5050 2100
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR059
U 1 1 5CD71353
P 6650 2100
F 0 "#PWR059" H 6650 1850 50  0001 C CNN
F 1 "GND" V 6655 1972 50  0000 R CNN
F 2 "" H 6650 2100 50  0001 C CNN
F 3 "" H 6650 2100 50  0001 C CNN
	1    6650 2100
	0    1    1    0   
$EndComp
Text GLabel 5050 2000 0    50   Input ~ 0
MOSI
Text GLabel 6650 2000 0    50   Input ~ 0
MOSI
Text GLabel 5050 1900 0    50   Input ~ 0
SCK
Text GLabel 6650 1900 0    50   Input ~ 0
SCK
Wire Wire Line
	6050 2000 6300 2000
Wire Wire Line
	6300 2000 6300 2200
Wire Wire Line
	6300 2200 7650 2200
Wire Wire Line
	7650 2200 7650 2100
$Comp
L kiyoshigawa_lib:otamatone OM2
U 1 1 5CD71361
P 8500 2000
F 0 "OM2" H 8828 2051 50  0000 L CNN
F 1 "otamatone" H 8828 1960 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x05_P2.54mm_Vertical" H 8650 2300 50  0001 C CNN
F 3 "" H 8650 2300 50  0001 C CNN
	1    8500 2000
	1    0    0    -1  
$EndComp
Wire Wire Line
	7650 2000 7750 2000
Wire Wire Line
	7750 2000 7750 1900
Wire Wire Line
	7750 1900 8400 1900
$Comp
L power:GND #PWR074
U 1 1 5CD7136B
P 8400 2200
F 0 "#PWR074" H 8400 1950 50  0001 C CNN
F 1 "GND" V 8405 2072 50  0000 R CNN
F 2 "" H 8400 2200 50  0001 C CNN
F 3 "" H 8400 2200 50  0001 C CNN
	1    8400 2200
	0    1    1    0   
$EndComp
$Comp
L power:+VDC #PWR073
U 1 1 5CD71371
P 8400 1800
F 0 "#PWR073" H 8400 1700 50  0001 C CNN
F 1 "+VDC" V 8400 2029 50  0000 L CNN
F 2 "" H 8400 1800 50  0001 C CNN
F 3 "" H 8400 1800 50  0001 C CNN
	1    8400 1800
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR033
U 1 1 5CD7137E
P 3800 1900
F 0 "#PWR033" H 3800 1650 50  0001 C CNN
F 1 "GND" V 3805 1772 50  0000 R CNN
F 2 "" H 3800 1900 50  0001 C CNN
F 3 "" H 3800 1900 50  0001 C CNN
	1    3800 1900
	0    1    1    0   
$EndComp
Wire Wire Line
	6050 2200 6050 2100
Wire Wire Line
	6050 2200 4700 2200
Wire Wire Line
	4550 2250 7800 2250
Wire Wire Line
	7800 2250 7800 2000
Wire Wire Line
	7800 2000 8400 2000
Text GLabel 8400 2100 0    50   Input ~ 0
A11
$Comp
L kiyoshigawa_lib:MCP-4151 U10
U 1 1 5CD71AC3
P 5550 2900
F 0 "U10" H 5550 3275 50  0000 C CNN
F 1 "MCP-4151" H 5550 3184 50  0000 C CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 5550 3250 50  0001 C CNN
F 3 "" H 5550 3250 50  0001 C CNN
	1    5550 2900
	1    0    0    -1  
$EndComp
$Comp
L kiyoshigawa_lib:MCP-4151 U16
U 1 1 5CD71ACA
P 7150 2900
F 0 "U16" H 7150 3275 50  0000 C CNN
F 1 "MCP-4151" H 7150 3184 50  0000 C CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 7150 3250 50  0001 C CNN
F 3 "" H 7150 3250 50  0001 C CNN
	1    7150 2900
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR052
U 1 1 5CD71AD1
P 6050 2750
F 0 "#PWR052" H 6050 2600 50  0001 C CNN
F 1 "+5V" V 6065 2878 50  0000 L CNN
F 2 "" H 6050 2750 50  0001 C CNN
F 3 "" H 6050 2750 50  0001 C CNN
	1    6050 2750
	0    1    1    0   
$EndComp
$Comp
L power:+5V #PWR067
U 1 1 5CD71AD7
P 7650 2750
F 0 "#PWR067" H 7650 2600 50  0001 C CNN
F 1 "+5V" V 7665 2878 50  0000 L CNN
F 2 "" H 7650 2750 50  0001 C CNN
F 3 "" H 7650 2750 50  0001 C CNN
	1    7650 2750
	0    1    1    0   
$EndComp
NoConn ~ 7650 2850
NoConn ~ 6050 2850
$Comp
L power:GND #PWR043
U 1 1 5CD71ADF
P 5050 3050
F 0 "#PWR043" H 5050 2800 50  0001 C CNN
F 1 "GND" V 5055 2922 50  0000 R CNN
F 2 "" H 5050 3050 50  0001 C CNN
F 3 "" H 5050 3050 50  0001 C CNN
	1    5050 3050
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR060
U 1 1 5CD71AE5
P 6650 3050
F 0 "#PWR060" H 6650 2800 50  0001 C CNN
F 1 "GND" V 6655 2922 50  0000 R CNN
F 2 "" H 6650 3050 50  0001 C CNN
F 3 "" H 6650 3050 50  0001 C CNN
	1    6650 3050
	0    1    1    0   
$EndComp
Text GLabel 5050 2950 0    50   Input ~ 0
MOSI
Text GLabel 6650 2950 0    50   Input ~ 0
MOSI
Text GLabel 5050 2850 0    50   Input ~ 0
SCK
Text GLabel 6650 2850 0    50   Input ~ 0
SCK
Wire Wire Line
	6050 2950 6300 2950
Wire Wire Line
	6300 2950 6300 3150
Wire Wire Line
	6300 3150 7650 3150
Wire Wire Line
	7650 3150 7650 3050
$Comp
L kiyoshigawa_lib:otamatone OM3
U 1 1 5CD71AF3
P 8500 2950
F 0 "OM3" H 8828 3001 50  0000 L CNN
F 1 "otamatone" H 8828 2910 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x05_P2.54mm_Vertical" H 8650 3250 50  0001 C CNN
F 3 "" H 8650 3250 50  0001 C CNN
	1    8500 2950
	1    0    0    -1  
$EndComp
Wire Wire Line
	7650 2950 7750 2950
Wire Wire Line
	7750 2950 7750 2850
Wire Wire Line
	7750 2850 8400 2850
$Comp
L power:GND #PWR076
U 1 1 5CD71AFD
P 8400 3150
F 0 "#PWR076" H 8400 2900 50  0001 C CNN
F 1 "GND" V 8405 3022 50  0000 R CNN
F 2 "" H 8400 3150 50  0001 C CNN
F 3 "" H 8400 3150 50  0001 C CNN
	1    8400 3150
	0    1    1    0   
$EndComp
$Comp
L power:+VDC #PWR075
U 1 1 5CD71B03
P 8400 2750
F 0 "#PWR075" H 8400 2650 50  0001 C CNN
F 1 "+VDC" V 8400 2979 50  0000 L CNN
F 2 "" H 8400 2750 50  0001 C CNN
F 3 "" H 8400 2750 50  0001 C CNN
	1    8400 2750
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6050 3150 6050 3050
Wire Wire Line
	4550 3200 7800 3200
Wire Wire Line
	7800 3200 7800 2950
Wire Wire Line
	7800 2950 8400 2950
$Comp
L kiyoshigawa_lib:Speaker S3
U 1 1 5CD71B1E
P 10500 3000
F 0 "S3" H 10778 3051 50  0000 L CNN
F 1 "Speaker" H 10778 2960 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 10500 3250 50  0001 C CNN
F 3 "" H 10500 3250 50  0001 C CNN
	1    10500 3000
	1    0    0    -1  
$EndComp
Text GLabel 8400 4000 0    50   Input ~ 0
A12
$Comp
L kiyoshigawa_lib:MCP-4151 U11
U 1 1 5CD72C64
P 5550 3850
F 0 "U11" H 5550 4225 50  0000 C CNN
F 1 "MCP-4151" H 5550 4134 50  0000 C CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 5550 4200 50  0001 C CNN
F 3 "" H 5550 4200 50  0001 C CNN
	1    5550 3850
	1    0    0    -1  
$EndComp
$Comp
L kiyoshigawa_lib:MCP-4151 U17
U 1 1 5CD72C6B
P 7150 3850
F 0 "U17" H 7150 4225 50  0000 C CNN
F 1 "MCP-4151" H 7150 4134 50  0000 C CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 7150 4200 50  0001 C CNN
F 3 "" H 7150 4200 50  0001 C CNN
	1    7150 3850
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR053
U 1 1 5CD72C72
P 6050 3700
F 0 "#PWR053" H 6050 3550 50  0001 C CNN
F 1 "+5V" V 6065 3828 50  0000 L CNN
F 2 "" H 6050 3700 50  0001 C CNN
F 3 "" H 6050 3700 50  0001 C CNN
	1    6050 3700
	0    1    1    0   
$EndComp
$Comp
L power:+5V #PWR068
U 1 1 5CD72C78
P 7650 3700
F 0 "#PWR068" H 7650 3550 50  0001 C CNN
F 1 "+5V" V 7665 3828 50  0000 L CNN
F 2 "" H 7650 3700 50  0001 C CNN
F 3 "" H 7650 3700 50  0001 C CNN
	1    7650 3700
	0    1    1    0   
$EndComp
NoConn ~ 7650 3800
NoConn ~ 6050 3800
$Comp
L power:GND #PWR044
U 1 1 5CD72C80
P 5050 4000
F 0 "#PWR044" H 5050 3750 50  0001 C CNN
F 1 "GND" V 5055 3872 50  0000 R CNN
F 2 "" H 5050 4000 50  0001 C CNN
F 3 "" H 5050 4000 50  0001 C CNN
	1    5050 4000
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR061
U 1 1 5CD72C86
P 6650 4000
F 0 "#PWR061" H 6650 3750 50  0001 C CNN
F 1 "GND" V 6655 3872 50  0000 R CNN
F 2 "" H 6650 4000 50  0001 C CNN
F 3 "" H 6650 4000 50  0001 C CNN
	1    6650 4000
	0    1    1    0   
$EndComp
Text GLabel 5050 3900 0    50   Input ~ 0
MOSI
Text GLabel 6650 3900 0    50   Input ~ 0
MOSI
Text GLabel 5050 3800 0    50   Input ~ 0
SCK
Text GLabel 6650 3800 0    50   Input ~ 0
SCK
Wire Wire Line
	6050 3900 6300 3900
Wire Wire Line
	6300 3900 6300 4100
Wire Wire Line
	6300 4100 7650 4100
Wire Wire Line
	7650 4100 7650 4000
$Comp
L kiyoshigawa_lib:otamatone OM4
U 1 1 5CD72C94
P 8500 3900
F 0 "OM4" H 8828 3951 50  0000 L CNN
F 1 "otamatone" H 8828 3860 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x05_P2.54mm_Vertical" H 8650 4200 50  0001 C CNN
F 3 "" H 8650 4200 50  0001 C CNN
	1    8500 3900
	1    0    0    -1  
$EndComp
Wire Wire Line
	7650 3900 7750 3900
Wire Wire Line
	7750 3900 7750 3800
Wire Wire Line
	7750 3800 8400 3800
$Comp
L power:GND #PWR078
U 1 1 5CD72C9E
P 8400 4100
F 0 "#PWR078" H 8400 3850 50  0001 C CNN
F 1 "GND" V 8405 3972 50  0000 R CNN
F 2 "" H 8400 4100 50  0001 C CNN
F 3 "" H 8400 4100 50  0001 C CNN
	1    8400 4100
	0    1    1    0   
$EndComp
$Comp
L power:+VDC #PWR077
U 1 1 5CD72CA4
P 8400 3700
F 0 "#PWR077" H 8400 3600 50  0001 C CNN
F 1 "+VDC" V 8400 3929 50  0000 L CNN
F 2 "" H 8400 3700 50  0001 C CNN
F 3 "" H 8400 3700 50  0001 C CNN
	1    8400 3700
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6050 4100 6050 4000
Wire Wire Line
	6050 4100 4700 4100
Wire Wire Line
	4550 4150 7800 4150
Wire Wire Line
	7800 4150 7800 3900
Wire Wire Line
	7800 3900 8400 3900
$Comp
L kiyoshigawa_lib:Speaker S4
U 1 1 5CD72CBF
P 10500 3600
F 0 "S4" H 10778 3651 50  0000 L CNN
F 1 "Speaker" H 10778 3560 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 10500 3850 50  0001 C CNN
F 3 "" H 10500 3850 50  0001 C CNN
	1    10500 3600
	1    0    0    -1  
$EndComp
Text GLabel 8400 3050 0    50   Input ~ 0
A13
$Comp
L kiyoshigawa_lib:MCP-4151 U12
U 1 1 5CD7483A
P 5550 4800
F 0 "U12" H 5550 5175 50  0000 C CNN
F 1 "MCP-4151" H 5550 5084 50  0000 C CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 5550 5150 50  0001 C CNN
F 3 "" H 5550 5150 50  0001 C CNN
	1    5550 4800
	1    0    0    -1  
$EndComp
$Comp
L kiyoshigawa_lib:MCP-4151 U18
U 1 1 5CD74841
P 7150 4800
F 0 "U18" H 7150 5175 50  0000 C CNN
F 1 "MCP-4151" H 7150 5084 50  0000 C CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 7150 5150 50  0001 C CNN
F 3 "" H 7150 5150 50  0001 C CNN
	1    7150 4800
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR054
U 1 1 5CD74848
P 6050 4650
F 0 "#PWR054" H 6050 4500 50  0001 C CNN
F 1 "+5V" V 6065 4778 50  0000 L CNN
F 2 "" H 6050 4650 50  0001 C CNN
F 3 "" H 6050 4650 50  0001 C CNN
	1    6050 4650
	0    1    1    0   
$EndComp
$Comp
L power:+5V #PWR069
U 1 1 5CD7484E
P 7650 4650
F 0 "#PWR069" H 7650 4500 50  0001 C CNN
F 1 "+5V" V 7665 4778 50  0000 L CNN
F 2 "" H 7650 4650 50  0001 C CNN
F 3 "" H 7650 4650 50  0001 C CNN
	1    7650 4650
	0    1    1    0   
$EndComp
NoConn ~ 7650 4750
NoConn ~ 6050 4750
$Comp
L power:GND #PWR045
U 1 1 5CD74856
P 5050 4950
F 0 "#PWR045" H 5050 4700 50  0001 C CNN
F 1 "GND" V 5055 4822 50  0000 R CNN
F 2 "" H 5050 4950 50  0001 C CNN
F 3 "" H 5050 4950 50  0001 C CNN
	1    5050 4950
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR062
U 1 1 5CD7485C
P 6650 4950
F 0 "#PWR062" H 6650 4700 50  0001 C CNN
F 1 "GND" V 6655 4822 50  0000 R CNN
F 2 "" H 6650 4950 50  0001 C CNN
F 3 "" H 6650 4950 50  0001 C CNN
	1    6650 4950
	0    1    1    0   
$EndComp
Text GLabel 5050 4850 0    50   Input ~ 0
MOSI
Text GLabel 6650 4850 0    50   Input ~ 0
MOSI
Text GLabel 5050 4750 0    50   Input ~ 0
SCK
Text GLabel 6650 4750 0    50   Input ~ 0
SCK
Wire Wire Line
	6050 4850 6300 4850
Wire Wire Line
	6300 4850 6300 5050
Wire Wire Line
	6300 5050 7650 5050
Wire Wire Line
	7650 5050 7650 4950
$Comp
L kiyoshigawa_lib:otamatone OM5
U 1 1 5CD7486A
P 8500 4850
F 0 "OM5" H 8828 4901 50  0000 L CNN
F 1 "otamatone" H 8828 4810 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x05_P2.54mm_Vertical" H 8650 5150 50  0001 C CNN
F 3 "" H 8650 5150 50  0001 C CNN
	1    8500 4850
	1    0    0    -1  
$EndComp
Wire Wire Line
	7650 4850 7750 4850
Wire Wire Line
	7750 4850 7750 4750
Wire Wire Line
	7750 4750 8400 4750
$Comp
L power:GND #PWR080
U 1 1 5CD74874
P 8400 5050
F 0 "#PWR080" H 8400 4800 50  0001 C CNN
F 1 "GND" V 8405 4922 50  0000 R CNN
F 2 "" H 8400 5050 50  0001 C CNN
F 3 "" H 8400 5050 50  0001 C CNN
	1    8400 5050
	0    1    1    0   
$EndComp
$Comp
L power:+VDC #PWR079
U 1 1 5CD7487A
P 8400 4650
F 0 "#PWR079" H 8400 4550 50  0001 C CNN
F 1 "+VDC" V 8400 4879 50  0000 L CNN
F 2 "" H 8400 4650 50  0001 C CNN
F 3 "" H 8400 4650 50  0001 C CNN
	1    8400 4650
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6050 5050 6050 4950
Wire Wire Line
	4550 5100 7800 5100
Wire Wire Line
	7800 5100 7800 4850
Wire Wire Line
	7800 4850 8400 4850
Text GLabel 8400 4950 0    50   Input ~ 0
A14
$Comp
L kiyoshigawa_lib:MCP-4151 U13
U 1 1 5CD774BF
P 5550 5750
F 0 "U13" H 5550 6125 50  0000 C CNN
F 1 "MCP-4151" H 5550 6034 50  0000 C CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 5550 6100 50  0001 C CNN
F 3 "" H 5550 6100 50  0001 C CNN
	1    5550 5750
	1    0    0    -1  
$EndComp
$Comp
L kiyoshigawa_lib:MCP-4151 U19
U 1 1 5CD774C6
P 7150 5750
F 0 "U19" H 7150 6125 50  0000 C CNN
F 1 "MCP-4151" H 7150 6034 50  0000 C CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 7150 6100 50  0001 C CNN
F 3 "" H 7150 6100 50  0001 C CNN
	1    7150 5750
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR055
U 1 1 5CD774CD
P 6050 5600
F 0 "#PWR055" H 6050 5450 50  0001 C CNN
F 1 "+5V" V 6065 5728 50  0000 L CNN
F 2 "" H 6050 5600 50  0001 C CNN
F 3 "" H 6050 5600 50  0001 C CNN
	1    6050 5600
	0    1    1    0   
$EndComp
$Comp
L power:+5V #PWR070
U 1 1 5CD774D3
P 7650 5600
F 0 "#PWR070" H 7650 5450 50  0001 C CNN
F 1 "+5V" V 7665 5728 50  0000 L CNN
F 2 "" H 7650 5600 50  0001 C CNN
F 3 "" H 7650 5600 50  0001 C CNN
	1    7650 5600
	0    1    1    0   
$EndComp
NoConn ~ 7650 5700
NoConn ~ 6050 5700
$Comp
L power:GND #PWR046
U 1 1 5CD774DB
P 5050 5900
F 0 "#PWR046" H 5050 5650 50  0001 C CNN
F 1 "GND" V 5055 5772 50  0000 R CNN
F 2 "" H 5050 5900 50  0001 C CNN
F 3 "" H 5050 5900 50  0001 C CNN
	1    5050 5900
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR063
U 1 1 5CD774E1
P 6650 5900
F 0 "#PWR063" H 6650 5650 50  0001 C CNN
F 1 "GND" V 6655 5772 50  0000 R CNN
F 2 "" H 6650 5900 50  0001 C CNN
F 3 "" H 6650 5900 50  0001 C CNN
	1    6650 5900
	0    1    1    0   
$EndComp
Text GLabel 5050 5800 0    50   Input ~ 0
MOSI
Text GLabel 6650 5800 0    50   Input ~ 0
MOSI
Text GLabel 5050 5700 0    50   Input ~ 0
SCK
Text GLabel 6650 5700 0    50   Input ~ 0
SCK
Wire Wire Line
	6050 5800 6300 5800
Wire Wire Line
	6300 5800 6300 6000
Wire Wire Line
	6300 6000 7650 6000
Wire Wire Line
	7650 6000 7650 5900
$Comp
L kiyoshigawa_lib:otamatone OM6
U 1 1 5CD774EF
P 8500 5800
F 0 "OM6" H 8828 5851 50  0000 L CNN
F 1 "otamatone" H 8828 5760 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x05_P2.54mm_Vertical" H 8650 6100 50  0001 C CNN
F 3 "" H 8650 6100 50  0001 C CNN
	1    8500 5800
	1    0    0    -1  
$EndComp
Wire Wire Line
	7650 5800 7750 5800
Wire Wire Line
	7750 5800 7750 5700
Wire Wire Line
	7750 5700 8400 5700
$Comp
L power:GND #PWR082
U 1 1 5CD774F9
P 8400 6000
F 0 "#PWR082" H 8400 5750 50  0001 C CNN
F 1 "GND" V 8405 5872 50  0000 R CNN
F 2 "" H 8400 6000 50  0001 C CNN
F 3 "" H 8400 6000 50  0001 C CNN
	1    8400 6000
	0    1    1    0   
$EndComp
$Comp
L power:+VDC #PWR081
U 1 1 5CD774FF
P 8400 5600
F 0 "#PWR081" H 8400 5500 50  0001 C CNN
F 1 "+VDC" V 8400 5829 50  0000 L CNN
F 2 "" H 8400 5600 50  0001 C CNN
F 3 "" H 8400 5600 50  0001 C CNN
	1    8400 5600
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6050 6000 6050 5900
Wire Wire Line
	6050 6000 4700 6000
Wire Wire Line
	4550 6050 7800 6050
Wire Wire Line
	7800 6050 7800 5800
Wire Wire Line
	7800 5800 8400 5800
Text GLabel 8400 5900 0    50   Input ~ 0
A0
Text GLabel 2850 2650 2    50   Input ~ 0
A10
Text GLabel 2850 2550 2    50   Input ~ 0
A11
Text GLabel 2850 2450 2    50   Input ~ 0
A12
Text GLabel 850  2650 0    50   Input ~ 0
A14
Text GLabel 2850 1150 2    50   Input ~ 0
A13
Text GLabel 850  2850 0    50   Input ~ 0
A0
$Comp
L kiyoshigawa_lib:PCA9685 J6
U 1 1 5CD98AA6
P 2650 5850
F 0 "J6" H 2878 5901 50  0000 L CNN
F 1 "PCA9685" H 2878 5810 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x06_P2.54mm_Vertical" H 2650 6300 50  0001 C CNN
F 3 "" H 2650 6300 50  0001 C CNN
	1    2650 5850
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR07
U 1 1 5CD9E20E
P 950 5550
F 0 "#PWR07" H 950 5400 50  0001 C CNN
F 1 "+3.3V" V 965 5678 50  0000 L CNN
F 2 "" H 950 5550 50  0001 C CNN
F 3 "" H 950 5550 50  0001 C CNN
	1    950  5550
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR021
U 1 1 5CDA231B
P 2550 5600
F 0 "#PWR021" H 2550 5350 50  0001 C CNN
F 1 "GND" V 2555 5472 50  0000 R CNN
F 2 "" H 2550 5600 50  0001 C CNN
F 3 "" H 2550 5600 50  0001 C CNN
	1    2550 5600
	0    1    1    0   
$EndComp
Text GLabel 850  3250 0    50   Input ~ 0
SDA
Text GLabel 850  3350 0    50   Input ~ 0
SCL
Text GLabel 5300 7550 0    50   Input ~ 0
SDA
Text GLabel 3750 7550 0    50   Input ~ 0
SCL
Text GLabel 850  3450 0    50   Input ~ 0
OE
Text GLabel 2250 7550 0    50   Input ~ 0
OE
$Comp
L Device:R R28
U 1 1 5CE3285F
P 10600 4000
F 0 "R28" V 10393 4000 50  0000 C CNN
F 1 "1k" V 10484 4000 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 10530 4000 50  0001 C CNN
F 3 "~" H 10600 4000 50  0001 C CNN
	1    10600 4000
	0    1    1    0   
$EndComp
$Comp
L Device:R R27
U 1 1 5CE361E6
P 10600 3400
F 0 "R27" V 10393 3400 50  0000 C CNN
F 1 "1k" V 10484 3400 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 10530 3400 50  0001 C CNN
F 3 "~" H 10600 3400 50  0001 C CNN
	1    10600 3400
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR092
U 1 1 5CE95CA8
P 10750 4000
F 0 "#PWR092" H 10750 3750 50  0001 C CNN
F 1 "GND" V 10755 3872 50  0000 R CNN
F 2 "" H 10750 4000 50  0001 C CNN
F 3 "" H 10750 4000 50  0001 C CNN
	1    10750 4000
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR091
U 1 1 5CE9A2BE
P 10750 3400
F 0 "#PWR091" H 10750 3150 50  0001 C CNN
F 1 "GND" V 10755 3272 50  0000 R CNN
F 2 "" H 10750 3400 50  0001 C CNN
F 3 "" H 10750 3400 50  0001 C CNN
	1    10750 3400
	0    -1   -1   0   
$EndComp
$Comp
L kiyoshigawa_lib:MIDI J3
U 1 1 5CF46D2B
P 2350 4050
F 0 "J3" H 2578 4101 50  0000 L CNN
F 1 "MIDI" H 2578 4010 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 2350 4400 50  0001 C CNN
F 3 "" H 2350 4400 50  0001 C CNN
	1    2350 4050
	1    0    0    -1  
$EndComp
Text GLabel 2250 4000 0    50   Input ~ 0
5v_D00
Text GLabel 2250 3900 0    50   Input ~ 0
5v_D01
Text GLabel 850  950  0    50   Input ~ 0
D00
Text GLabel 850  1050 0    50   Input ~ 0
D01
$Comp
L power:+5V #PWR015
U 1 1 5CF47489
P 2250 4100
F 0 "#PWR015" H 2250 3950 50  0001 C CNN
F 1 "+5V" V 2265 4228 50  0000 L CNN
F 2 "" H 2250 4100 50  0001 C CNN
F 3 "" H 2250 4100 50  0001 C CNN
	1    2250 4100
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR016
U 1 1 5CF585C8
P 2250 4200
F 0 "#PWR016" H 2250 3950 50  0001 C CNN
F 1 "GND" V 2255 4072 50  0000 R CNN
F 2 "" H 2250 4200 50  0001 C CNN
F 3 "" H 2250 4200 50  0001 C CNN
	1    2250 4200
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR048
U 1 1 5CF6A22C
P 4650 6300
F 0 "#PWR048" H 4650 6050 50  0001 C CNN
F 1 "GND" V 4655 6172 50  0000 R CNN
F 2 "" H 4650 6300 50  0001 C CNN
F 3 "" H 4650 6300 50  0001 C CNN
	1    4650 6300
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR057
U 1 1 5CF7FABA
P 5750 6300
F 0 "#PWR057" H 5750 6050 50  0001 C CNN
F 1 "GND" V 5755 6172 50  0000 R CNN
F 2 "" H 5750 6300 50  0001 C CNN
F 3 "" H 5750 6300 50  0001 C CNN
	1    5750 6300
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR049
U 1 1 5CF83EFB
P 4850 6550
F 0 "#PWR049" H 4850 6300 50  0001 C CNN
F 1 "GND" V 4855 6422 50  0000 R CNN
F 2 "" H 4850 6550 50  0001 C CNN
F 3 "" H 4850 6550 50  0001 C CNN
	1    4850 6550
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR064
U 1 1 5CF83F08
P 5950 6550
F 0 "#PWR064" H 5950 6300 50  0001 C CNN
F 1 "GND" V 5955 6422 50  0000 R CNN
F 2 "" H 5950 6550 50  0001 C CNN
F 3 "" H 5950 6550 50  0001 C CNN
	1    5950 6550
	1    0    0    -1  
$EndComp
$Comp
L Connector:Screw_Terminal_01x02 J4
U 1 1 5CFC0F01
P 2400 3650
F 0 "J4" H 2479 3642 50  0000 L CNN
F 1 "Otamatone Power" H 2479 3551 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 2400 3650 50  0001 C CNN
F 3 "~" H 2400 3650 50  0001 C CNN
	1    2400 3650
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR014
U 1 1 5CFC104E
P 2200 3750
F 0 "#PWR014" H 2200 3500 50  0001 C CNN
F 1 "GND" V 2205 3622 50  0000 R CNN
F 2 "" H 2200 3750 50  0001 C CNN
F 3 "" H 2200 3750 50  0001 C CNN
	1    2200 3750
	0    1    1    0   
$EndComp
$Comp
L power:+VDC #PWR013
U 1 1 5CFC532E
P 2200 3650
F 0 "#PWR013" H 2200 3550 50  0001 C CNN
F 1 "+VDC" V 2200 3879 50  0000 L CNN
F 2 "" H 2200 3650 50  0001 C CNN
F 3 "" H 2200 3650 50  0001 C CNN
	1    2200 3650
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR06
U 1 1 5CFFA891
P 950 4700
F 0 "#PWR06" H 950 4450 50  0001 C CNN
F 1 "GND" V 955 4572 50  0000 R CNN
F 2 "" H 950 4700 50  0001 C CNN
F 3 "" H 950 4700 50  0001 C CNN
	1    950  4700
	0    1    1    0   
$EndComp
$Comp
L power:+VDC #PWR05
U 1 1 5CFFF144
P 950 4400
F 0 "#PWR05" H 950 4300 50  0001 C CNN
F 1 "+VDC" V 950 4629 50  0000 L CNN
F 2 "" H 950 4400 50  0001 C CNN
F 3 "" H 950 4400 50  0001 C CNN
	1    950  4400
	0    -1   -1   0   
$EndComp
$Comp
L Connector:Screw_Terminal_01x02 J2
U 1 1 5D07C28A
P 1350 3650
F 0 "J2" H 1430 3642 50  0000 L CNN
F 1 "LED Power" H 1430 3551 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 1350 3650 50  0001 C CNN
F 3 "~" H 1350 3650 50  0001 C CNN
	1    1350 3650
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR09
U 1 1 5D07C291
P 1150 3750
F 0 "#PWR09" H 1150 3500 50  0001 C CNN
F 1 "GND" V 1155 3622 50  0000 R CNN
F 2 "" H 1150 3750 50  0001 C CNN
F 3 "" H 1150 3750 50  0001 C CNN
	1    1150 3750
	0    1    1    0   
$EndComp
$Comp
L kiyoshigawa_lib:LED_Strip J1
U 1 1 5D099971
P 1300 4100
F 0 "J1" H 1527 4151 50  0000 L CNN
F 1 "LED_Strip" H 1527 4060 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 1300 4400 50  0001 C CNN
F 3 "" H 1300 4400 50  0001 C CNN
	1    1300 4100
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR010
U 1 1 5D099B0E
P 1200 4200
F 0 "#PWR010" H 1200 3950 50  0001 C CNN
F 1 "GND" V 1205 4072 50  0000 R CNN
F 2 "" H 1200 4200 50  0001 C CNN
F 3 "" H 1200 4200 50  0001 C CNN
	1    1200 4200
	0    1    1    0   
$EndComp
Text GLabel 850  1150 0    50   Input ~ 0
D02
Text GLabel 750  7550 0    50   Input ~ 0
D02
Text GLabel 1150 3650 0    50   Output ~ 0
VLED
Text GLabel 1200 4000 0    50   Input ~ 0
VLED
Text GLabel 850  1250 0    50   Input ~ 0
D03
Text GLabel 850  1350 0    50   Input ~ 0
D04
Text GLabel 850  1450 0    50   Input ~ 0
D05
Text GLabel 850  1550 0    50   Input ~ 0
D06
Text GLabel 850  1650 0    50   Input ~ 0
D07
Text GLabel 850  1750 0    50   Input ~ 0
D08
Text GLabel 850  1850 0    50   Input ~ 0
D09
Text GLabel 850  1950 0    50   Input ~ 0
D10
Text GLabel 850  2950 0    50   Input ~ 0
D15
Text GLabel 850  3050 0    50   Input ~ 0
D16
Text GLabel 850  3150 0    50   Input ~ 0
D17
Text GLabel 2850 3450 2    50   Input ~ 0
D21
Text GLabel 2850 3350 2    50   Input ~ 0
D22
Text GLabel 2850 3250 2    50   Input ~ 0
D23
Text GLabel 2850 2250 2    50   Input ~ 0
D24
Text GLabel 2850 2150 2    50   Input ~ 0
D25
Text GLabel 2850 2050 2    50   Input ~ 0
D26
Text GLabel 2850 1950 2    50   Input ~ 0
D27
Text GLabel 2850 1850 2    50   Input ~ 0
D28
Text GLabel 2850 1650 2    50   Input ~ 0
D30
Text GLabel 2850 1750 2    50   Input ~ 0
D29
Text GLabel 2850 1550 2    50   Input ~ 0
D31
Text GLabel 2850 1450 2    50   Input ~ 0
D32
Text GLabel 2850 1350 2    50   Input ~ 0
D33
$Comp
L SparkFun-Capacitors:1000UF-RADIAL-5MM-35V-20%-KIT C4
U 1 1 5D0EFAD7
P 2600 4500
F 0 "C4" H 2728 4545 45  0000 L CNN
F 1 "1000UF" H 2728 4461 45  0000 L CNN
F 2 "Capacitors:CPOL-RADIAL-5MM-10MM-KIT" H 2600 4750 20  0001 C CNN
F 3 "" H 2600 4500 50  0001 C CNN
F 4 "" H 2728 4366 60  0000 L CNN "Field4"
	1    2600 4500
	1    0    0    -1  
$EndComp
$Comp
L SparkFun-Capacitors:1000UF-RADIAL-5MM-35V-20%-KIT C3
U 1 1 5D0EFC4E
P 2100 4500
F 0 "C3" H 2228 4545 45  0000 L CNN
F 1 "1000UF" H 2228 4461 45  0000 L CNN
F 2 "Capacitors:CPOL-RADIAL-5MM-10MM-KIT" H 2100 4750 20  0001 C CNN
F 3 "" H 2100 4500 50  0001 C CNN
F 4 "" H 2228 4366 60  0000 L CNN "Field4"
	1    2100 4500
	1    0    0    -1  
$EndComp
$Comp
L SparkFun-Capacitors:1000UF-RADIAL-5MM-35V-20%-KIT C2
U 1 1 5D0F3E6C
P 1600 4500
F 0 "C2" H 1728 4545 45  0000 L CNN
F 1 "1000UF" H 1728 4461 45  0000 L CNN
F 2 "Capacitors:CPOL-RADIAL-5MM-10MM-KIT" H 1600 4750 20  0001 C CNN
F 3 "" H 1600 4500 50  0001 C CNN
F 4 "" H 1728 4366 60  0000 L CNN "Field4"
	1    1600 4500
	1    0    0    -1  
$EndComp
$Comp
L SparkFun-Capacitors:1000UF-RADIAL-5MM-35V-20%-KIT C1
U 1 1 5D0F8085
P 1100 4500
F 0 "C1" H 1228 4545 45  0000 L CNN
F 1 "1000UF" H 1228 4461 45  0000 L CNN
F 2 "Capacitors:CPOL-RADIAL-5MM-10MM-KIT" H 1100 4750 20  0001 C CNN
F 3 "" H 1100 4500 50  0001 C CNN
F 4 "" H 1228 4366 60  0000 L CNN "Field4"
	1    1100 4500
	1    0    0    -1  
$EndComp
Wire Wire Line
	950  4400 1100 4400
Connection ~ 1100 4400
Wire Wire Line
	950  4700 1100 4700
Connection ~ 1100 4700
Wire Wire Line
	1100 4400 1600 4400
Wire Wire Line
	1100 4700 1600 4700
Text GLabel 6650 950  0    50   Input ~ 0
SCK
Text GLabel 10400 1150 0    50   Input ~ 0
A10
Text GLabel 10400 1750 0    50   Input ~ 0
A11
Text GLabel 10400 3050 0    50   Input ~ 0
A13
Text GLabel 10400 3650 0    50   Input ~ 0
A12
Wire Wire Line
	10750 3750 10750 4000
Connection ~ 10750 4000
Wire Wire Line
	10750 3150 10750 3400
Connection ~ 10750 3400
Text GLabel 9200 5150 0    50   Input ~ 0
D28
Text GLabel 8950 3550 0    50   Input ~ 0
D29
Text GLabel 8950 5450 0    50   Input ~ 0
D27
Text GLabel 9200 3250 0    50   Input ~ 0
D30
Text GLabel 9200 1350 0    50   Input ~ 0
D26
Text GLabel 8950 1650 0    50   Input ~ 0
D25
Text GLabel 3500 5600 0    50   Input ~ 0
D31
Text GLabel 3500 5150 0    50   Input ~ 0
D32
Text GLabel 3500 1800 0    50   Input ~ 0
D33
Text GLabel 3500 3250 0    50   Input ~ 0
D24
Text GLabel 3500 1350 0    50   Input ~ 0
D15
Text GLabel 3500 3700 0    50   Input ~ 0
D10
Text GLabel 5050 5600 0    50   Input ~ 0
D05
Text GLabel 6650 5600 0    50   Input ~ 0
D04
Text GLabel 6650 4650 0    50   Input ~ 0
D06
Text GLabel 5050 4650 0    50   Input ~ 0
D07
Text GLabel 6650 3700 0    50   Input ~ 0
D08
Text GLabel 5050 3700 0    50   Input ~ 0
D09
Text GLabel 5050 2750 0    50   Input ~ 0
D03
Text GLabel 6650 2750 0    50   Input ~ 0
D23
Text GLabel 5050 1800 0    50   Input ~ 0
D22
Text GLabel 6650 1800 0    50   Input ~ 0
D21
Text GLabel 5050 850  0    50   Input ~ 0
D17
Text GLabel 6650 850  0    50   Input ~ 0
D16
$Comp
L SparkFun-DiscreteSemi:MOSFET-NCH-BSS138 Q1
U 1 1 5CE5A189
P 1250 7450
F 0 "Q1" V 1457 7450 45  0000 C CNN
F 1 "BSS138" V 1541 7450 45  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 1250 7700 20  0001 C CNN
F 3 "" H 1250 7450 50  0001 C CNN
F 4 "" V 1636 7450 60  0000 C CNN "Field4"
	1    1250 7450
	0    1    1    0   
$EndComp
Wire Wire Line
	750  7550 850  7550
Wire Wire Line
	1450 7550 1500 7550
$Comp
L Device:R R1
U 1 1 5CE6F004
P 850 7400
F 0 "R1" V 643 7400 50  0000 C CNN
F 1 "10k" V 734 7400 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 780 7400 50  0001 C CNN
F 3 "~" H 850 7400 50  0001 C CNN
	1    850  7400
	-1   0    0    1   
$EndComp
Connection ~ 850  7550
Wire Wire Line
	850  7550 1050 7550
$Comp
L Device:R R2
U 1 1 5CE7F0C1
P 1500 7400
F 0 "R2" V 1293 7400 50  0000 C CNN
F 1 "10k" V 1384 7400 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 1430 7400 50  0001 C CNN
F 3 "~" H 1500 7400 50  0001 C CNN
	1    1500 7400
	-1   0    0    1   
$EndComp
$Comp
L power:+5V #PWR011
U 1 1 5CE8A498
P 1500 7250
F 0 "#PWR011" H 1500 7100 50  0001 C CNN
F 1 "+5V" V 1515 7378 50  0000 L CNN
F 2 "" H 1500 7250 50  0001 C CNN
F 3 "" H 1500 7250 50  0001 C CNN
	1    1500 7250
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR04
U 1 1 5CEA619E
P 850 7150
F 0 "#PWR04" H 850 7000 50  0001 C CNN
F 1 "+3.3V" V 865 7278 50  0000 L CNN
F 2 "" H 850 7150 50  0001 C CNN
F 3 "" H 850 7150 50  0001 C CNN
	1    850  7150
	0    -1   -1   0   
$EndComp
Wire Wire Line
	850  7150 850  7250
Wire Wire Line
	850  7150 1150 7150
Wire Wire Line
	1150 7150 1150 7250
Connection ~ 850  7150
Text GLabel 1600 7550 2    50   Input ~ 0
5v_D02
Wire Wire Line
	1500 7550 1600 7550
Connection ~ 1500 7550
Text GLabel 1200 4100 0    50   Input ~ 0
5v_D02
$Comp
L SparkFun-DiscreteSemi:MOSFET-NCH-BSS138 Q3
U 1 1 5CF49C66
P 2750 7450
F 0 "Q3" V 2957 7450 45  0000 C CNN
F 1 "BSS138" V 3041 7450 45  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 2750 7700 20  0001 C CNN
F 3 "" H 2750 7450 50  0001 C CNN
F 4 "" V 3136 7450 60  0000 C CNN "Field4"
	1    2750 7450
	0    1    1    0   
$EndComp
Wire Wire Line
	2250 7550 2350 7550
Wire Wire Line
	2950 7550 3000 7550
$Comp
L Device:R R5
U 1 1 5CF49C6F
P 2350 7400
F 0 "R5" V 2143 7400 50  0000 C CNN
F 1 "10k" V 2234 7400 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 2280 7400 50  0001 C CNN
F 3 "~" H 2350 7400 50  0001 C CNN
	1    2350 7400
	-1   0    0    1   
$EndComp
Connection ~ 2350 7550
Wire Wire Line
	2350 7550 2550 7550
$Comp
L Device:R R6
U 1 1 5CF49C78
P 3000 7400
F 0 "R6" V 2793 7400 50  0000 C CNN
F 1 "10k" V 2884 7400 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 2930 7400 50  0001 C CNN
F 3 "~" H 3000 7400 50  0001 C CNN
	1    3000 7400
	-1   0    0    1   
$EndComp
$Comp
L power:+5V #PWR027
U 1 1 5CF49C7F
P 3000 7250
F 0 "#PWR027" H 3000 7100 50  0001 C CNN
F 1 "+5V" V 3015 7378 50  0000 L CNN
F 2 "" H 3000 7250 50  0001 C CNN
F 3 "" H 3000 7250 50  0001 C CNN
	1    3000 7250
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR018
U 1 1 5CF49C85
P 2350 7150
F 0 "#PWR018" H 2350 7000 50  0001 C CNN
F 1 "+3.3V" V 2365 7278 50  0000 L CNN
F 2 "" H 2350 7150 50  0001 C CNN
F 3 "" H 2350 7150 50  0001 C CNN
	1    2350 7150
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2350 7150 2350 7250
Wire Wire Line
	2350 7150 2650 7150
Wire Wire Line
	2650 7150 2650 7250
Connection ~ 2350 7150
Wire Wire Line
	3000 7550 3100 7550
Connection ~ 3000 7550
$Comp
L SparkFun-DiscreteSemi:MOSFET-NCH-BSS138 Q5
U 1 1 5CF50484
P 4250 7450
F 0 "Q5" V 4457 7450 45  0000 C CNN
F 1 "BSS138" V 4541 7450 45  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 4250 7700 20  0001 C CNN
F 3 "" H 4250 7450 50  0001 C CNN
F 4 "" V 4636 7450 60  0000 C CNN "Field4"
	1    4250 7450
	0    1    1    0   
$EndComp
Wire Wire Line
	3750 7550 3850 7550
Wire Wire Line
	4450 7550 4500 7550
$Comp
L Device:R R14
U 1 1 5CF5048D
P 3850 7400
F 0 "R14" V 3643 7400 50  0000 C CNN
F 1 "10k" V 3734 7400 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 3780 7400 50  0001 C CNN
F 3 "~" H 3850 7400 50  0001 C CNN
	1    3850 7400
	-1   0    0    1   
$EndComp
Connection ~ 3850 7550
Wire Wire Line
	3850 7550 4050 7550
$Comp
L Device:R R16
U 1 1 5CF50496
P 4500 7400
F 0 "R16" V 4293 7400 50  0000 C CNN
F 1 "10k" V 4384 7400 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 4430 7400 50  0001 C CNN
F 3 "~" H 4500 7400 50  0001 C CNN
	1    4500 7400
	-1   0    0    1   
$EndComp
$Comp
L power:+5V #PWR040
U 1 1 5CF5049D
P 4500 7250
F 0 "#PWR040" H 4500 7100 50  0001 C CNN
F 1 "+5V" V 4515 7378 50  0000 L CNN
F 2 "" H 4500 7250 50  0001 C CNN
F 3 "" H 4500 7250 50  0001 C CNN
	1    4500 7250
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR038
U 1 1 5CF504A3
P 3850 7150
F 0 "#PWR038" H 3850 7000 50  0001 C CNN
F 1 "+3.3V" V 3865 7278 50  0000 L CNN
F 2 "" H 3850 7150 50  0001 C CNN
F 3 "" H 3850 7150 50  0001 C CNN
	1    3850 7150
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3850 7150 3850 7250
Wire Wire Line
	3850 7150 4150 7150
Wire Wire Line
	4150 7150 4150 7250
Connection ~ 3850 7150
Wire Wire Line
	4500 7550 4600 7550
Connection ~ 4500 7550
$Comp
L SparkFun-DiscreteSemi:MOSFET-NCH-BSS138 Q6
U 1 1 5CF83249
P 5800 7450
F 0 "Q6" V 6007 7450 45  0000 C CNN
F 1 "BSS138" V 6091 7450 45  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 5800 7700 20  0001 C CNN
F 3 "" H 5800 7450 50  0001 C CNN
F 4 "" V 6186 7450 60  0000 C CNN "Field4"
	1    5800 7450
	0    1    1    0   
$EndComp
Wire Wire Line
	5300 7550 5400 7550
Wire Wire Line
	6000 7550 6050 7550
$Comp
L Device:R R17
U 1 1 5CF83252
P 5400 7400
F 0 "R17" V 5193 7400 50  0000 C CNN
F 1 "10k" V 5284 7400 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 5330 7400 50  0001 C CNN
F 3 "~" H 5400 7400 50  0001 C CNN
	1    5400 7400
	-1   0    0    1   
$EndComp
Connection ~ 5400 7550
Wire Wire Line
	5400 7550 5600 7550
$Comp
L Device:R R18
U 1 1 5CF8325B
P 6050 7400
F 0 "R18" V 5843 7400 50  0000 C CNN
F 1 "10k" V 5934 7400 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 5980 7400 50  0001 C CNN
F 3 "~" H 6050 7400 50  0001 C CNN
	1    6050 7400
	-1   0    0    1   
$EndComp
$Comp
L power:+5V #PWR056
U 1 1 5CF83262
P 6050 7250
F 0 "#PWR056" H 6050 7100 50  0001 C CNN
F 1 "+5V" V 6065 7378 50  0000 L CNN
F 2 "" H 6050 7250 50  0001 C CNN
F 3 "" H 6050 7250 50  0001 C CNN
	1    6050 7250
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR047
U 1 1 5CF83268
P 5400 7150
F 0 "#PWR047" H 5400 7000 50  0001 C CNN
F 1 "+3.3V" V 5415 7278 50  0000 L CNN
F 2 "" H 5400 7150 50  0001 C CNN
F 3 "" H 5400 7150 50  0001 C CNN
	1    5400 7150
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5400 7150 5400 7250
Wire Wire Line
	5400 7150 5700 7150
Wire Wire Line
	5700 7150 5700 7250
Connection ~ 5400 7150
Wire Wire Line
	6050 7550 6150 7550
Connection ~ 6050 7550
Text GLabel 3100 7550 2    50   Input ~ 0
5v_OE
Text GLabel 4600 7550 2    50   Input ~ 0
5v_SCL
Text GLabel 6150 7550 2    50   Input ~ 0
5v_SDA
Connection ~ 1600 4400
Connection ~ 1600 4700
Wire Wire Line
	1600 4400 2100 4400
Wire Wire Line
	1600 4700 2100 4700
Connection ~ 2100 4400
Connection ~ 2100 4700
Wire Wire Line
	2100 4400 2600 4400
Wire Wire Line
	2100 4700 2600 4700
$Comp
L power:+5V #PWR08
U 1 1 5D09B09B
P 950 5950
F 0 "#PWR08" H 950 5800 50  0001 C CNN
F 1 "+5V" V 965 6078 50  0000 L CNN
F 2 "" H 950 5950 50  0001 C CNN
F 3 "" H 950 5950 50  0001 C CNN
	1    950  5950
	0    -1   -1   0   
$EndComp
Wire Wire Line
	1100 5750 1100 6000
Wire Wire Line
	1100 6000 2550 6000
Text GLabel 850  5000 0    50   Input ~ 0
SDA
Wire Wire Line
	850  5000 950  5000
Text GLabel 850  5400 0    50   Input ~ 0
5v_SDA
Wire Wire Line
	850  5400 950  5400
Wire Wire Line
	1100 5200 1200 5200
Wire Wire Line
	1200 5200 1200 5900
Wire Wire Line
	1200 5900 2550 5900
Text GLabel 1700 4800 0    50   Input ~ 0
OE
Wire Wire Line
	1700 4800 1800 4800
Text GLabel 1700 5200 0    50   Input ~ 0
5v_OE
Wire Wire Line
	1700 5200 1800 5200
Text GLabel 1700 5350 0    50   Input ~ 0
SCL
Wire Wire Line
	1700 5350 1800 5350
Text GLabel 1700 5750 0    50   Input ~ 0
5v_SCL
Wire Wire Line
	1700 5750 1800 5750
Wire Wire Line
	1950 5550 1950 5800
Wire Wire Line
	1950 5800 2550 5800
Wire Wire Line
	2550 5700 2050 5700
Wire Wire Line
	2050 5700 2050 5000
Wire Wire Line
	2050 5000 1950 5000
Text GLabel 1500 6800 0    50   Input ~ 0
D00
$Comp
L SparkFun-DiscreteSemi:MOSFET-NCH-BSS138 Q2
U 1 1 5D2366FE
P 2000 6700
F 0 "Q2" V 2207 6700 45  0000 C CNN
F 1 "BSS138" V 2291 6700 45  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 2000 6950 20  0001 C CNN
F 3 "" H 2000 6700 50  0001 C CNN
F 4 "" V 2386 6700 60  0000 C CNN "Field4"
	1    2000 6700
	0    1    1    0   
$EndComp
Wire Wire Line
	1500 6800 1600 6800
Wire Wire Line
	2200 6800 2250 6800
$Comp
L Device:R R3
U 1 1 5D236707
P 1600 6650
F 0 "R3" V 1393 6650 50  0000 C CNN
F 1 "10k" V 1484 6650 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 1530 6650 50  0001 C CNN
F 3 "~" H 1600 6650 50  0001 C CNN
	1    1600 6650
	-1   0    0    1   
$EndComp
Connection ~ 1600 6800
Wire Wire Line
	1600 6800 1800 6800
$Comp
L Device:R R4
U 1 1 5D236710
P 2250 6650
F 0 "R4" V 2043 6650 50  0000 C CNN
F 1 "10k" V 2134 6650 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 2180 6650 50  0001 C CNN
F 3 "~" H 2250 6650 50  0001 C CNN
	1    2250 6650
	-1   0    0    1   
$EndComp
$Comp
L power:+5V #PWR017
U 1 1 5D236717
P 2250 6500
F 0 "#PWR017" H 2250 6350 50  0001 C CNN
F 1 "+5V" V 2265 6628 50  0000 L CNN
F 2 "" H 2250 6500 50  0001 C CNN
F 3 "" H 2250 6500 50  0001 C CNN
	1    2250 6500
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR012
U 1 1 5D23671D
P 1600 6400
F 0 "#PWR012" H 1600 6250 50  0001 C CNN
F 1 "+3.3V" V 1615 6528 50  0000 L CNN
F 2 "" H 1600 6400 50  0001 C CNN
F 3 "" H 1600 6400 50  0001 C CNN
	1    1600 6400
	0    -1   -1   0   
$EndComp
Wire Wire Line
	1600 6400 1600 6500
Wire Wire Line
	1600 6400 1900 6400
Wire Wire Line
	1900 6400 1900 6500
Connection ~ 1600 6400
Text GLabel 2350 6800 2    50   Input ~ 0
5v_D00
Wire Wire Line
	2250 6800 2350 6800
Connection ~ 2250 6800
Text GLabel 3450 6800 0    50   Input ~ 0
D01
$Comp
L SparkFun-DiscreteSemi:MOSFET-NCH-BSS138 Q4
U 1 1 5D24181F
P 3950 6700
F 0 "Q4" V 4157 6700 45  0000 C CNN
F 1 "BSS138" V 4241 6700 45  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 3950 6950 20  0001 C CNN
F 3 "" H 3950 6700 50  0001 C CNN
F 4 "" V 4336 6700 60  0000 C CNN "Field4"
	1    3950 6700
	0    1    1    0   
$EndComp
Wire Wire Line
	3450 6800 3550 6800
Wire Wire Line
	4150 6800 4200 6800
$Comp
L Device:R R7
U 1 1 5D241828
P 3550 6650
F 0 "R7" V 3343 6650 50  0000 C CNN
F 1 "10k" V 3434 6650 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 3480 6650 50  0001 C CNN
F 3 "~" H 3550 6650 50  0001 C CNN
	1    3550 6650
	-1   0    0    1   
$EndComp
Connection ~ 3550 6800
Wire Wire Line
	3550 6800 3750 6800
$Comp
L Device:R R15
U 1 1 5D241831
P 4200 6650
F 0 "R15" V 3993 6650 50  0000 C CNN
F 1 "10k" V 4084 6650 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 4130 6650 50  0001 C CNN
F 3 "~" H 4200 6650 50  0001 C CNN
	1    4200 6650
	-1   0    0    1   
$EndComp
$Comp
L power:+5V #PWR039
U 1 1 5D241838
P 4200 6500
F 0 "#PWR039" H 4200 6350 50  0001 C CNN
F 1 "+5V" V 4215 6628 50  0000 L CNN
F 2 "" H 4200 6500 50  0001 C CNN
F 3 "" H 4200 6500 50  0001 C CNN
	1    4200 6500
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR029
U 1 1 5D24183E
P 3550 6400
F 0 "#PWR029" H 3550 6250 50  0001 C CNN
F 1 "+3.3V" V 3565 6528 50  0000 L CNN
F 2 "" H 3550 6400 50  0001 C CNN
F 3 "" H 3550 6400 50  0001 C CNN
	1    3550 6400
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3550 6400 3550 6500
Wire Wire Line
	3550 6400 3850 6400
Wire Wire Line
	3850 6400 3850 6500
Connection ~ 3550 6400
Text GLabel 4300 6800 2    50   Input ~ 0
5v_D01
Wire Wire Line
	4200 6800 4300 6800
Connection ~ 4200 6800
$Comp
L power:+3.3V #PWR022
U 1 1 5CD71045
P 2850 1250
F 0 "#PWR022" H 2850 1100 50  0001 C CNN
F 1 "+3.3V" V 2865 1378 50  0000 L CNN
F 2 "" H 2850 1250 50  0001 C CNN
F 3 "" H 2850 1250 50  0001 C CNN
	1    2850 1250
	0    1    1    0   
$EndComp
$Comp
L power:+3.3V #PWR026
U 1 1 5CD6E9F6
P 2850 3150
F 0 "#PWR026" H 2850 3000 50  0001 C CNN
F 1 "+3.3V" V 2865 3278 50  0000 L CNN
F 2 "" H 2850 3150 50  0001 C CNN
F 3 "" H 2850 3150 50  0001 C CNN
	1    2850 3150
	0    1    1    0   
$EndComp
$Comp
L power:+3.3V #PWR02
U 1 1 5CD7101C
P 850 2350
F 0 "#PWR02" H 850 2200 50  0001 C CNN
F 1 "+3.3V" V 865 2478 50  0000 L CNN
F 2 "" H 850 2350 50  0001 C CNN
F 3 "" H 850 2350 50  0001 C CNN
	1    850  2350
	0    -1   -1   0   
$EndComp
$Comp
L power:+VDC #PWR028
U 1 1 5D2C0F54
P 3100 4550
F 0 "#PWR028" H 3100 4450 50  0001 C CNN
F 1 "+VDC" V 3100 4779 50  0000 L CNN
F 2 "" H 3100 4550 50  0001 C CNN
F 3 "" H 3100 4550 50  0001 C CNN
	1    3100 4550
	-1   0    0    1   
$EndComp
$Comp
L power:PWR_FLAG #FLG01
U 1 1 5D2CCF7E
P 3100 4450
F 0 "#FLG01" H 3100 4525 50  0001 C CNN
F 1 "PWR_FLAG" V 3100 4578 50  0000 L CNN
F 2 "" H 3100 4450 50  0001 C CNN
F 3 "~" H 3100 4450 50  0001 C CNN
	1    3100 4450
	1    0    0    -1  
$EndComp
Wire Wire Line
	3100 4550 3100 4450
NoConn ~ 2550 6100
NoConn ~ 5800 4800
$Comp
L kiyoshigawa_lib:I2C_HEADER J5
U 1 1 5CEAEC3B
P 2650 5150
F 0 "J5" H 2878 5201 50  0000 L CNN
F 1 "3v3_I2C" H 2878 5110 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 2650 5500 50  0001 C CNN
F 3 "" H 2650 5500 50  0001 C CNN
	1    2650 5150
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR020
U 1 1 5CEAEC42
P 2550 5300
F 0 "#PWR020" H 2550 5050 50  0001 C CNN
F 1 "GND" V 2555 5172 50  0000 R CNN
F 2 "" H 2550 5300 50  0001 C CNN
F 3 "" H 2550 5300 50  0001 C CNN
	1    2550 5300
	0    1    1    0   
$EndComp
$Comp
L power:+3.3V #PWR019
U 1 1 5CEBB223
P 2550 5200
F 0 "#PWR019" H 2550 5050 50  0001 C CNN
F 1 "+3.3V" V 2565 5328 50  0000 L CNN
F 2 "" H 2550 5200 50  0001 C CNN
F 3 "" H 2550 5200 50  0001 C CNN
	1    2550 5200
	0    -1   -1   0   
$EndComp
Text GLabel 2550 5000 0    50   Input ~ 0
SDA
Text GLabel 2550 5100 0    50   Input ~ 0
SCL
$Comp
L SparkFun-DiscreteSemi:DUAL-OPTOISOLATOR_SOIC8 U2
U 1 1 5CEC8174
P 4250 1600
F 0 "U2" H 4200 2054 45  0000 C CNN
F 1 "DUAL-OPTO" H 4200 1970 45  0000 C CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 4250 2050 20  0001 C CNN
F 3 "" H 4250 1600 50  0001 C CNN
F 4 "" H 4200 1981 60  0000 C CNN "Field4"
	1    4250 1600
	1    0    0    -1  
$EndComp
Wire Wire Line
	4450 1250 4450 1400
Wire Wire Line
	4450 1250 6050 1250
Wire Wire Line
	4550 1300 4550 1500
Wire Wire Line
	4550 1500 4450 1500
Wire Wire Line
	4700 2200 4700 1600
Wire Wire Line
	4700 1600 4450 1600
Wire Wire Line
	4550 2250 4550 1700
Wire Wire Line
	4550 1700 4450 1700
$Comp
L Device:R R8
U 1 1 5CFE6B5F
P 3650 1350
F 0 "R8" V 3443 1350 50  0000 C CNN
F 1 "270R" V 3534 1350 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 3580 1350 50  0001 C CNN
F 3 "~" H 3650 1350 50  0001 C CNN
	1    3650 1350
	0    1    1    0   
$EndComp
$Comp
L Device:R R9
U 1 1 5D03BCED
P 3650 1800
F 0 "R9" V 3443 1800 50  0000 C CNN
F 1 "270R" V 3534 1800 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 3580 1800 50  0001 C CNN
F 3 "~" H 3650 1800 50  0001 C CNN
	1    3650 1800
	0    1    1    0   
$EndComp
Wire Wire Line
	3800 1500 3950 1500
Wire Wire Line
	3950 1700 3950 1900
Wire Wire Line
	3850 1800 3850 1600
Wire Wire Line
	3850 1600 3950 1600
Wire Wire Line
	3950 1400 3800 1400
Wire Wire Line
	3800 1400 3800 1350
$Comp
L power:GND #PWR036
U 1 1 5D1688CF
P 3800 5300
F 0 "#PWR036" H 3800 5050 50  0001 C CNN
F 1 "GND" V 3805 5172 50  0000 R CNN
F 2 "" H 3800 5300 50  0001 C CNN
F 3 "" H 3800 5300 50  0001 C CNN
	1    3800 5300
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR037
U 1 1 5D1688D5
P 3800 5700
F 0 "#PWR037" H 3800 5450 50  0001 C CNN
F 1 "GND" V 3805 5572 50  0000 R CNN
F 2 "" H 3800 5700 50  0001 C CNN
F 3 "" H 3800 5700 50  0001 C CNN
	1    3800 5700
	0    1    1    0   
$EndComp
$Comp
L SparkFun-DiscreteSemi:DUAL-OPTOISOLATOR_SOIC8 U4
U 1 1 5D1688DC
P 4250 5400
F 0 "U4" H 4200 5854 45  0000 C CNN
F 1 "DUAL-OPTO" H 4200 5770 45  0000 C CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 4250 5850 20  0001 C CNN
F 3 "" H 4250 5400 50  0001 C CNN
F 4 "" H 4200 5781 60  0000 C CNN "Field4"
	1    4250 5400
	1    0    0    -1  
$EndComp
$Comp
L Device:R R12
U 1 1 5D1688E3
P 3650 5150
F 0 "R12" V 3443 5150 50  0000 C CNN
F 1 "270R" V 3534 5150 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 3580 5150 50  0001 C CNN
F 3 "~" H 3650 5150 50  0001 C CNN
	1    3650 5150
	0    1    1    0   
$EndComp
$Comp
L Device:R R13
U 1 1 5D1688EA
P 3650 5600
F 0 "R13" V 3443 5600 50  0000 C CNN
F 1 "270R" V 3534 5600 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 3580 5600 50  0001 C CNN
F 3 "~" H 3650 5600 50  0001 C CNN
	1    3650 5600
	0    1    1    0   
$EndComp
Wire Wire Line
	3800 5300 3950 5300
Wire Wire Line
	3950 5500 3950 5700
Wire Wire Line
	3850 5600 3850 5400
Wire Wire Line
	3850 5400 3950 5400
Wire Wire Line
	3950 5200 3800 5200
Wire Wire Line
	3800 5200 3800 5150
$Comp
L power:GND #PWR034
U 1 1 5D1769D8
P 3800 3400
F 0 "#PWR034" H 3800 3150 50  0001 C CNN
F 1 "GND" V 3805 3272 50  0000 R CNN
F 2 "" H 3800 3400 50  0001 C CNN
F 3 "" H 3800 3400 50  0001 C CNN
	1    3800 3400
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR035
U 1 1 5D1769DE
P 3800 3800
F 0 "#PWR035" H 3800 3550 50  0001 C CNN
F 1 "GND" V 3805 3672 50  0000 R CNN
F 2 "" H 3800 3800 50  0001 C CNN
F 3 "" H 3800 3800 50  0001 C CNN
	1    3800 3800
	0    1    1    0   
$EndComp
$Comp
L SparkFun-DiscreteSemi:DUAL-OPTOISOLATOR_SOIC8 U3
U 1 1 5D1769E5
P 4250 3500
F 0 "U3" H 4200 3954 45  0000 C CNN
F 1 "DUAL-OPTO" H 4200 3870 45  0000 C CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 4250 3950 20  0001 C CNN
F 3 "" H 4250 3500 50  0001 C CNN
F 4 "" H 4200 3881 60  0000 C CNN "Field4"
	1    4250 3500
	1    0    0    -1  
$EndComp
$Comp
L Device:R R10
U 1 1 5D1769EC
P 3650 3250
F 0 "R10" V 3443 3250 50  0000 C CNN
F 1 "270R" V 3534 3250 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 3580 3250 50  0001 C CNN
F 3 "~" H 3650 3250 50  0001 C CNN
	1    3650 3250
	0    1    1    0   
$EndComp
$Comp
L Device:R R11
U 1 1 5D1769F3
P 3650 3700
F 0 "R11" V 3443 3700 50  0000 C CNN
F 1 "270R" V 3534 3700 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 3580 3700 50  0001 C CNN
F 3 "~" H 3650 3700 50  0001 C CNN
	1    3650 3700
	0    1    1    0   
$EndComp
Wire Wire Line
	3800 3400 3950 3400
Wire Wire Line
	3950 3600 3950 3800
Wire Wire Line
	3850 3700 3850 3500
Wire Wire Line
	3850 3500 3950 3500
Wire Wire Line
	3950 3300 3800 3300
Wire Wire Line
	3800 3300 3800 3250
Wire Wire Line
	4700 6000 4700 5400
Wire Wire Line
	4700 5400 4450 5400
Wire Wire Line
	4550 6050 4550 5500
Wire Wire Line
	4550 5500 4450 5500
Wire Wire Line
	4550 5100 4550 5300
Wire Wire Line
	4550 5300 4450 5300
Wire Wire Line
	4450 5200 4450 5050
Wire Wire Line
	4450 5050 6050 5050
Wire Wire Line
	4550 4150 4550 3600
Wire Wire Line
	4550 3600 4450 3600
Wire Wire Line
	4700 4100 4700 3500
Wire Wire Line
	4700 3500 4450 3500
Wire Wire Line
	4550 3200 4550 3400
Wire Wire Line
	4550 3400 4450 3400
Wire Wire Line
	4450 3300 4450 3150
Wire Wire Line
	4450 3150 6050 3150
Wire Wire Line
	3850 5600 3800 5600
Wire Wire Line
	3800 5700 3950 5700
Wire Wire Line
	3850 3700 3800 3700
Wire Wire Line
	3800 3800 3950 3800
Wire Wire Line
	3850 1800 3800 1800
Wire Wire Line
	3800 1900 3950 1900
Text GLabel 10450 1500 0    50   Input ~ 0
A10
Text GLabel 10450 2100 0    50   Input ~ 0
A11
Text GLabel 10450 3400 0    50   Input ~ 0
A13
Text GLabel 10450 4000 0    50   Input ~ 0
A12
$Comp
L power:GND #PWR085
U 1 1 5D730D5B
P 9550 3450
F 0 "#PWR085" H 9550 3200 50  0001 C CNN
F 1 "GND" V 9555 3322 50  0000 R CNN
F 2 "" H 9550 3450 50  0001 C CNN
F 3 "" H 9550 3450 50  0001 C CNN
	1    9550 3450
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR086
U 1 1 5D730D61
P 9550 3650
F 0 "#PWR086" H 9550 3400 50  0001 C CNN
F 1 "GND" V 9555 3522 50  0000 R CNN
F 2 "" H 9550 3650 50  0001 C CNN
F 3 "" H 9550 3650 50  0001 C CNN
	1    9550 3650
	0    1    1    0   
$EndComp
$Comp
L SparkFun-DiscreteSemi:DUAL-OPTOISOLATOR_SOIC8 U6
U 1 1 5D730D68
P 9850 3550
F 0 "U6" H 9800 4004 45  0000 C CNN
F 1 "DUAL-OPTO" H 9800 3920 45  0000 C CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 9850 4000 20  0001 C CNN
F 3 "" H 9850 3550 50  0001 C CNN
F 4 "" H 9800 3931 60  0000 C CNN "Field4"
	1    9850 3550
	1    0    0    -1  
$EndComp
$Comp
L Device:R R23
U 1 1 5D730D71
P 9350 3250
F 0 "R23" V 9143 3250 50  0000 C CNN
F 1 "270R" V 9234 3250 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 9280 3250 50  0001 C CNN
F 3 "~" H 9350 3250 50  0001 C CNN
	1    9350 3250
	0    1    1    0   
$EndComp
$Comp
L Device:R R20
U 1 1 5D730D79
P 9100 3550
F 0 "R20" V 8893 3550 50  0000 C CNN
F 1 "270R" V 8984 3550 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 9030 3550 50  0001 C CNN
F 3 "~" H 9100 3550 50  0001 C CNN
	1    9100 3550
	0    1    1    0   
$EndComp
Wire Wire Line
	9550 3550 9250 3550
Wire Wire Line
	10050 3350 10050 2950
Wire Wire Line
	10050 2950 10400 2950
Wire Wire Line
	10150 3150 10150 3450
Wire Wire Line
	10150 3450 10050 3450
Wire Wire Line
	10150 3150 10750 3150
Wire Wire Line
	10050 3750 10750 3750
Wire Wire Line
	10050 3750 10050 3650
Wire Wire Line
	10050 3550 10400 3550
Wire Wire Line
	9500 3250 9550 3250
Wire Wire Line
	9550 3250 9550 3350
$Comp
L kiyoshigawa_lib:Speaker S1
U 1 1 5D97F100
P 10500 1100
F 0 "S1" H 10778 1151 50  0000 L CNN
F 1 "Speaker" H 10778 1060 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 10500 1350 50  0001 C CNN
F 3 "" H 10500 1350 50  0001 C CNN
	1    10500 1100
	1    0    0    -1  
$EndComp
$Comp
L kiyoshigawa_lib:Speaker S2
U 1 1 5D97F107
P 10500 1700
F 0 "S2" H 10778 1751 50  0000 L CNN
F 1 "Speaker" H 10778 1660 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 10500 1950 50  0001 C CNN
F 3 "" H 10500 1950 50  0001 C CNN
	1    10500 1700
	1    0    0    -1  
$EndComp
$Comp
L Device:R R26
U 1 1 5D97F10E
P 10600 2100
F 0 "R26" V 10393 2100 50  0000 C CNN
F 1 "1k" V 10484 2100 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 10530 2100 50  0001 C CNN
F 3 "~" H 10600 2100 50  0001 C CNN
	1    10600 2100
	0    1    1    0   
$EndComp
$Comp
L Device:R R25
U 1 1 5D97F115
P 10600 1500
F 0 "R25" V 10393 1500 50  0000 C CNN
F 1 "1k" V 10484 1500 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 10530 1500 50  0001 C CNN
F 3 "~" H 10600 1500 50  0001 C CNN
	1    10600 1500
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR090
U 1 1 5D97F11C
P 10750 2100
F 0 "#PWR090" H 10750 1850 50  0001 C CNN
F 1 "GND" V 10755 1972 50  0000 R CNN
F 2 "" H 10750 2100 50  0001 C CNN
F 3 "" H 10750 2100 50  0001 C CNN
	1    10750 2100
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR089
U 1 1 5D97F122
P 10750 1500
F 0 "#PWR089" H 10750 1250 50  0001 C CNN
F 1 "GND" V 10755 1372 50  0000 R CNN
F 2 "" H 10750 1500 50  0001 C CNN
F 3 "" H 10750 1500 50  0001 C CNN
	1    10750 1500
	0    -1   -1   0   
$EndComp
Wire Wire Line
	10750 1850 10750 2100
Connection ~ 10750 2100
Wire Wire Line
	10750 1250 10750 1500
Connection ~ 10750 1500
$Comp
L power:GND #PWR083
U 1 1 5D97F131
P 9550 1550
F 0 "#PWR083" H 9550 1300 50  0001 C CNN
F 1 "GND" V 9555 1422 50  0000 R CNN
F 2 "" H 9550 1550 50  0001 C CNN
F 3 "" H 9550 1550 50  0001 C CNN
	1    9550 1550
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR084
U 1 1 5D97F137
P 9550 1750
F 0 "#PWR084" H 9550 1500 50  0001 C CNN
F 1 "GND" V 9555 1622 50  0000 R CNN
F 2 "" H 9550 1750 50  0001 C CNN
F 3 "" H 9550 1750 50  0001 C CNN
	1    9550 1750
	0    1    1    0   
$EndComp
$Comp
L SparkFun-DiscreteSemi:DUAL-OPTOISOLATOR_SOIC8 U5
U 1 1 5D97F13E
P 9850 1650
F 0 "U5" H 9800 2104 45  0000 C CNN
F 1 "DUAL-OPTO" H 9800 2020 45  0000 C CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 9850 2100 20  0001 C CNN
F 3 "" H 9850 1650 50  0001 C CNN
F 4 "" H 9800 2031 60  0000 C CNN "Field4"
	1    9850 1650
	1    0    0    -1  
$EndComp
$Comp
L Device:R R22
U 1 1 5D97F145
P 9350 1350
F 0 "R22" V 9143 1350 50  0000 C CNN
F 1 "270R" V 9234 1350 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 9280 1350 50  0001 C CNN
F 3 "~" H 9350 1350 50  0001 C CNN
	1    9350 1350
	0    1    1    0   
$EndComp
$Comp
L Device:R R19
U 1 1 5D97F14C
P 9100 1650
F 0 "R19" V 8893 1650 50  0000 C CNN
F 1 "270R" V 8984 1650 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 9030 1650 50  0001 C CNN
F 3 "~" H 9100 1650 50  0001 C CNN
	1    9100 1650
	0    1    1    0   
$EndComp
Wire Wire Line
	9550 1650 9250 1650
Wire Wire Line
	10050 1450 10050 1050
Wire Wire Line
	10050 1050 10400 1050
Wire Wire Line
	10150 1250 10150 1550
Wire Wire Line
	10150 1550 10050 1550
Wire Wire Line
	10150 1250 10750 1250
Wire Wire Line
	10050 1850 10750 1850
Wire Wire Line
	10050 1850 10050 1750
Wire Wire Line
	10050 1650 10400 1650
Wire Wire Line
	9500 1350 9550 1350
Wire Wire Line
	9550 1350 9550 1450
$Comp
L kiyoshigawa_lib:Speaker S5
U 1 1 5DA0FFA5
P 10500 4900
F 0 "S5" H 10778 4951 50  0000 L CNN
F 1 "Speaker" H 10778 4860 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 10500 5150 50  0001 C CNN
F 3 "" H 10500 5150 50  0001 C CNN
	1    10500 4900
	1    0    0    -1  
$EndComp
$Comp
L kiyoshigawa_lib:Speaker S6
U 1 1 5DA0FFAC
P 10500 5500
F 0 "S6" H 10778 5551 50  0000 L CNN
F 1 "Speaker" H 10778 5460 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 10500 5750 50  0001 C CNN
F 3 "" H 10500 5750 50  0001 C CNN
	1    10500 5500
	1    0    0    -1  
$EndComp
$Comp
L Device:R R30
U 1 1 5DA0FFB3
P 10600 5900
F 0 "R30" V 10393 5900 50  0000 C CNN
F 1 "1k" V 10484 5900 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 10530 5900 50  0001 C CNN
F 3 "~" H 10600 5900 50  0001 C CNN
	1    10600 5900
	0    1    1    0   
$EndComp
$Comp
L Device:R R29
U 1 1 5DA0FFBA
P 10600 5300
F 0 "R29" V 10393 5300 50  0000 C CNN
F 1 "1k" V 10484 5300 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 10530 5300 50  0001 C CNN
F 3 "~" H 10600 5300 50  0001 C CNN
	1    10600 5300
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR094
U 1 1 5DA0FFC1
P 10750 5900
F 0 "#PWR094" H 10750 5650 50  0001 C CNN
F 1 "GND" V 10755 5772 50  0000 R CNN
F 2 "" H 10750 5900 50  0001 C CNN
F 3 "" H 10750 5900 50  0001 C CNN
	1    10750 5900
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR093
U 1 1 5DA0FFC7
P 10750 5300
F 0 "#PWR093" H 10750 5050 50  0001 C CNN
F 1 "GND" V 10755 5172 50  0000 R CNN
F 2 "" H 10750 5300 50  0001 C CNN
F 3 "" H 10750 5300 50  0001 C CNN
	1    10750 5300
	0    -1   -1   0   
$EndComp
Wire Wire Line
	10750 5650 10750 5900
Connection ~ 10750 5900
Wire Wire Line
	10750 5050 10750 5300
Connection ~ 10750 5300
$Comp
L power:GND #PWR087
U 1 1 5DA0FFD6
P 9550 5350
F 0 "#PWR087" H 9550 5100 50  0001 C CNN
F 1 "GND" V 9555 5222 50  0000 R CNN
F 2 "" H 9550 5350 50  0001 C CNN
F 3 "" H 9550 5350 50  0001 C CNN
	1    9550 5350
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR088
U 1 1 5DA0FFDC
P 9550 5550
F 0 "#PWR088" H 9550 5300 50  0001 C CNN
F 1 "GND" V 9555 5422 50  0000 R CNN
F 2 "" H 9550 5550 50  0001 C CNN
F 3 "" H 9550 5550 50  0001 C CNN
	1    9550 5550
	0    1    1    0   
$EndComp
$Comp
L SparkFun-DiscreteSemi:DUAL-OPTOISOLATOR_SOIC8 U7
U 1 1 5DA0FFE3
P 9850 5450
F 0 "U7" H 9800 5904 45  0000 C CNN
F 1 "DUAL-OPTO" H 9800 5820 45  0000 C CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 9850 5900 20  0001 C CNN
F 3 "" H 9850 5450 50  0001 C CNN
F 4 "" H 9800 5831 60  0000 C CNN "Field4"
	1    9850 5450
	1    0    0    -1  
$EndComp
$Comp
L Device:R R24
U 1 1 5DA0FFEA
P 9350 5150
F 0 "R24" V 9143 5150 50  0000 C CNN
F 1 "270R" V 9234 5150 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 9280 5150 50  0001 C CNN
F 3 "~" H 9350 5150 50  0001 C CNN
	1    9350 5150
	0    1    1    0   
$EndComp
$Comp
L Device:R R21
U 1 1 5DA0FFF1
P 9100 5450
F 0 "R21" V 8893 5450 50  0000 C CNN
F 1 "270R" V 8984 5450 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 9030 5450 50  0001 C CNN
F 3 "~" H 9100 5450 50  0001 C CNN
	1    9100 5450
	0    1    1    0   
$EndComp
Wire Wire Line
	9550 5450 9250 5450
Wire Wire Line
	10050 5250 10050 4850
Wire Wire Line
	10050 4850 10400 4850
Wire Wire Line
	10150 5050 10150 5350
Wire Wire Line
	10150 5350 10050 5350
Wire Wire Line
	10150 5050 10750 5050
Wire Wire Line
	10050 5650 10750 5650
Wire Wire Line
	10050 5650 10050 5550
Wire Wire Line
	10050 5450 10400 5450
Wire Wire Line
	9500 5150 9550 5150
Wire Wire Line
	9550 5150 9550 5250
Text GLabel 10450 5900 0    50   Input ~ 0
A0
Text GLabel 10400 5550 0    50   Input ~ 0
A0
Text GLabel 10400 4950 0    50   Input ~ 0
A14
Text GLabel 10450 5300 0    50   Input ~ 0
A14
$Comp
L kiyoshigawa_lib:I2C_HEADER J7
U 1 1 5DA9F833
P 3750 4400
F 0 "J7" H 3978 4451 50  0000 L CNN
F 1 "5v_I2C" H 3978 4360 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 3750 4750 50  0001 C CNN
F 3 "" H 3750 4750 50  0001 C CNN
	1    3750 4400
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR031
U 1 1 5DA9F83A
P 3650 4550
F 0 "#PWR031" H 3650 4300 50  0001 C CNN
F 1 "GND" V 3655 4422 50  0000 R CNN
F 2 "" H 3650 4550 50  0001 C CNN
F 3 "" H 3650 4550 50  0001 C CNN
	1    3650 4550
	0    1    1    0   
$EndComp
$Comp
L power:+5V #PWR030
U 1 1 5DABBAE3
P 3650 4450
F 0 "#PWR030" H 3650 4300 50  0001 C CNN
F 1 "+5V" V 3665 4578 50  0000 L CNN
F 2 "" H 3650 4450 50  0001 C CNN
F 3 "" H 3650 4450 50  0001 C CNN
	1    3650 4450
	0    -1   -1   0   
$EndComp
Text GLabel 3650 4250 0    50   Input ~ 0
5v_SDA
Text GLabel 3650 4350 0    50   Input ~ 0
5v_SCL
Text GLabel 7250 6850 0    50   Input ~ 0
D01
Text GLabel 7250 6650 0    50   Input ~ 0
D00
Text GLabel 8150 6650 0    50   Input ~ 0
D02
Text GLabel 8150 6850 0    50   Input ~ 0
OE
Text GLabel 850  2150 0    50   Input ~ 0
MISO
$Comp
L kiyoshigawa_lib:SPI_Out J8
U 1 1 5CF7E827
P 3650 2500
F 0 "J8" H 3977 2551 50  0000 L CNN
F 1 "SPI_Out" H 3977 2460 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x05_P2.54mm_Vertical" H 3800 2800 50  0001 C CNN
F 3 "" H 3800 2800 50  0001 C CNN
	1    3650 2500
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR096
U 1 1 5CF7EA07
P 3550 2700
F 0 "#PWR096" H 3550 2450 50  0001 C CNN
F 1 "GND" V 3555 2572 50  0000 R CNN
F 2 "" H 3550 2700 50  0001 C CNN
F 3 "" H 3550 2700 50  0001 C CNN
	1    3550 2700
	0    1    1    0   
$EndComp
$Comp
L power:+3.3V #PWR095
U 1 1 5CF7EA0D
P 3550 2600
F 0 "#PWR095" H 3550 2450 50  0001 C CNN
F 1 "+3.3V" V 3565 2728 50  0000 L CNN
F 2 "" H 3550 2600 50  0001 C CNN
F 3 "" H 3550 2600 50  0001 C CNN
	1    3550 2600
	0    -1   -1   0   
$EndComp
Text GLabel 3550 2300 0    50   Input ~ 0
MOSI
Text GLabel 3550 2400 0    50   Input ~ 0
MISO
Text GLabel 3550 2500 0    50   Input ~ 0
SCK
$Comp
L oMIDItone-V2-rescue:MountingHole_Pad-Mechanical H1
U 1 1 5CEB3E9F
P 4650 6200
F 0 "H1" H 4750 6249 50  0000 L CNN
F 1 "MountingHole_Pad" H 4750 6158 50  0000 L CNN
F 2 "MountingHole:MountingHole_3mm_Pad_Via" H 4650 6200 50  0001 C CNN
F 3 "~" H 4650 6200 50  0001 C CNN
	1    4650 6200
	1    0    0    -1  
$EndComp
$Comp
L oMIDItone-V2-rescue:MountingHole_Pad-Mechanical H2
U 1 1 5CEB5339
P 4850 6450
F 0 "H2" H 4950 6499 50  0000 L CNN
F 1 "MountingHole_Pad" H 4950 6408 50  0000 L CNN
F 2 "MountingHole:MountingHole_3mm_Pad_Via" H 4850 6450 50  0001 C CNN
F 3 "~" H 4850 6450 50  0001 C CNN
	1    4850 6450
	1    0    0    -1  
$EndComp
$Comp
L oMIDItone-V2-rescue:MountingHole_Pad-Mechanical H3
U 1 1 5CEC3731
P 5750 6200
F 0 "H3" H 5850 6249 50  0000 L CNN
F 1 "MountingHole_Pad" H 5850 6158 50  0000 L CNN
F 2 "MountingHole:MountingHole_3mm_Pad_Via" H 5750 6200 50  0001 C CNN
F 3 "~" H 5750 6200 50  0001 C CNN
	1    5750 6200
	1    0    0    -1  
$EndComp
$Comp
L oMIDItone-V2-rescue:MountingHole_Pad-Mechanical H4
U 1 1 5CED1BB2
P 5950 6450
F 0 "H4" H 6050 6499 50  0000 L CNN
F 1 "MountingHole_Pad" H 6050 6408 50  0000 L CNN
F 2 "MountingHole:MountingHole_3mm_Pad_Via" H 5950 6450 50  0001 C CNN
F 3 "~" H 5950 6450 50  0001 C CNN
	1    5950 6450
	1    0    0    -1  
$EndComp
$Comp
L oMIDItone-V2-rescue:TestPoint-Connector TP1
U 1 1 5CF6F7EE
P 7250 6650
F 0 "TP1" V 7204 6838 50  0000 L CNN
F 1 "TestPoint" V 7295 6838 50  0000 L CNN
F 2 "Measurement_Points:Test_Point_Keystone_5019_Minature" H 7450 6650 50  0001 C CNN
F 3 "~" H 7450 6650 50  0001 C CNN
	1    7250 6650
	0    1    1    0   
$EndComp
$Comp
L oMIDItone-V2-rescue:TestPoint-Connector TP2
U 1 1 5CF702ED
P 7250 6850
F 0 "TP2" V 7204 7038 50  0000 L CNN
F 1 "TestPoint" V 7295 7038 50  0000 L CNN
F 2 "Measurement_Points:Test_Point_Keystone_5019_Minature" H 7450 6850 50  0001 C CNN
F 3 "~" H 7450 6850 50  0001 C CNN
	1    7250 6850
	0    1    1    0   
$EndComp
$Comp
L oMIDItone-V2-rescue:TestPoint-Connector TP3
U 1 1 5CF71D61
P 8150 6650
F 0 "TP3" V 8104 6838 50  0000 L CNN
F 1 "TestPoint" V 8195 6838 50  0000 L CNN
F 2 "Measurement_Points:Test_Point_Keystone_5019_Minature" H 8350 6650 50  0001 C CNN
F 3 "~" H 8350 6650 50  0001 C CNN
	1    8150 6650
	0    1    1    0   
$EndComp
$Comp
L oMIDItone-V2-rescue:TestPoint-Connector TP4
U 1 1 5CF73F00
P 8150 6850
F 0 "TP4" V 8104 7038 50  0000 L CNN
F 1 "TestPoint" V 8195 7038 50  0000 L CNN
F 2 "Measurement_Points:Test_Point_Keystone_5019_Minature" H 8350 6850 50  0001 C CNN
F 3 "~" H 8350 6850 50  0001 C CNN
	1    8150 6850
	0    1    1    0   
$EndComp
$Comp
L Jumper:SolderJumper_3_Bridged12 JP1
U 1 1 5CEFB1C1
P 950 5200
F 0 "JP1" V 996 5267 50  0000 L CNN
F 1 "SRVO_SDA" V 905 5267 50  0000 L CNN
F 2 "Jumper:SolderJumper-3_P1.3mm_Bridged12_Pad1.0x1.5mm" H 950 5200 50  0001 C CNN
F 3 "~" H 950 5200 50  0001 C CNN
	1    950  5200
	0    -1   -1   0   
$EndComp
$Comp
L Jumper:SolderJumper_3_Bridged12 JP2
U 1 1 5CF0E171
P 950 5750
F 0 "JP2" V 996 5817 50  0000 L CNN
F 1 "SRVO_VOLT" V 905 5817 50  0000 L CNN
F 2 "Jumper:SolderJumper-3_P1.3mm_Bridged12_Pad1.0x1.5mm" H 950 5750 50  0001 C CNN
F 3 "~" H 950 5750 50  0001 C CNN
	1    950  5750
	0    -1   -1   0   
$EndComp
$Comp
L Jumper:SolderJumper_3_Bridged12 JP3
U 1 1 5CF473BF
P 1800 5000
F 0 "JP3" V 1846 5067 50  0000 L CNN
F 1 "SRVO_OE" V 1755 5067 50  0000 L CNN
F 2 "Jumper:SolderJumper-3_P1.3mm_Bridged12_Pad1.0x1.5mm" H 1800 5000 50  0001 C CNN
F 3 "~" H 1800 5000 50  0001 C CNN
	1    1800 5000
	0    -1   -1   0   
$EndComp
$Comp
L Jumper:SolderJumper_3_Bridged12 JP4
U 1 1 5CF63983
P 1800 5550
F 0 "JP4" V 1846 5617 50  0000 L CNN
F 1 "SRVO_SCL" V 1755 5617 50  0000 L CNN
F 2 "Jumper:SolderJumper-3_P1.3mm_Bridged12_Pad1.0x1.5mm" H 1800 5550 50  0001 C CNN
F 3 "~" H 1800 5550 50  0001 C CNN
	1    1800 5550
	0    -1   -1   0   
$EndComp
$EndSCHEMATC
