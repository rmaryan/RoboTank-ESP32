EESchema Schematic File Version 4
LIBS:RoboTank-ESP32-cache
EELAYER 29 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "RoboTank-ESP32"
Date "2019-08-25"
Rev "1.0"
Comp "Mar'yan Rachynskyy"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L !mrach_custom:HX-2S-02 U2
U 1 1 5D62725B
P 2700 1350
F 0 "U2" H 2700 1675 50  0000 C CNN
F 1 "HX-2S-D01" H 2700 1584 50  0000 C CNN
F 2 "" H 2700 1350 50  0001 C CNN
F 3 "" H 2700 1350 50  0001 C CNN
	1    2700 1350
	1    0    0    -1  
$EndComp
$Comp
L power:+7.5V #PWR01
U 1 1 5D627C48
P 1050 850
F 0 "#PWR01" H 1050 700 50  0001 C CNN
F 1 "+7.5V" H 1065 1023 50  0000 C CNN
F 2 "" H 1050 850 50  0001 C CNN
F 3 "" H 1050 850 50  0001 C CNN
	1    1050 850 
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR02
U 1 1 5D6285C7
P 1050 1050
F 0 "#PWR02" H 1050 800 50  0001 C CNN
F 1 "GND" H 1055 877 50  0000 C CNN
F 2 "" H 1050 1050 50  0001 C CNN
F 3 "" H 1050 1050 50  0001 C CNN
	1    1050 1050
	1    0    0    -1  
$EndComp
Text Notes 600  950  0    50   ~ 0
Charger 1
Wire Wire Line
	1050 1050 1250 1050
$Comp
L Switch:SW_SPDT SW1
U 1 1 5D62DEB0
P 3550 1250
F 0 "SW1" H 3550 1535 50  0000 C CNN
F 1 "Main Switch" H 3550 1444 50  0000 C CNN
F 2 "" H 3550 1250 50  0001 C CNN
F 3 "~" H 3550 1250 50  0001 C CNN
	1    3550 1250
	1    0    0    -1  
$EndComp
Wire Wire Line
	3200 1250 3350 1250
Wire Wire Line
	1050 850  3850 850 
Wire Wire Line
	1250 1800 3200 1800
Wire Wire Line
	3200 1800 3200 1450
Wire Wire Line
	1250 1050 1250 1800
$Comp
L Device:Battery_Cell BT1
U 1 1 5D631D31
P 1500 1200
F 0 "BT1" H 1618 1296 50  0000 L CNN
F 1 "LiIon 18650" H 1618 1205 50  0000 L CNN
F 2 "" V 1500 1260 50  0001 C CNN
F 3 "~" V 1500 1260 50  0001 C CNN
	1    1500 1200
	1    0    0    -1  
$EndComp
$Comp
L Device:Battery_Cell BT2
U 1 1 5D6328C2
P 1500 1600
F 0 "BT2" H 1618 1696 50  0000 L CNN
F 1 "LiIon 18650" H 1618 1605 50  0000 L CNN
F 2 "" V 1500 1660 50  0001 C CNN
F 3 "~" V 1500 1660 50  0001 C CNN
	1    1500 1600
	1    0    0    -1  
$EndComp
Wire Wire Line
	1500 1300 1500 1350
Wire Wire Line
	1500 1000 2200 1000
Wire Wire Line
	1500 1700 2200 1700
Wire Wire Line
	2200 1700 2200 1450
Connection ~ 1500 1350
Wire Wire Line
	1500 1350 1500 1400
Wire Wire Line
	2200 1000 2200 1250
Wire Wire Line
	1500 1350 2200 1350
Wire Wire Line
	3750 1150 3850 1150
Wire Wire Line
	3850 1150 3850 850 
Wire Notes Line
	3900 1950 550  1950
Wire Notes Line
	550  1950 550  550 
Wire Notes Line
	550  550  3900 550 
Wire Notes Line
	3900 550  3900 1950
Text Notes 1850 700  0    50   ~ 0
Main Power Supply
$Comp
L !mrach_custom:HX-2S-02 U3
U 1 1 5D63D355
P 2700 2850
F 0 "U3" H 2700 3175 50  0000 C CNN
F 1 "HX-2S-D01" H 2700 3084 50  0000 C CNN
F 2 "" H 2700 2850 50  0001 C CNN
F 3 "" H 2700 2850 50  0001 C CNN
	1    2700 2850
	1    0    0    -1  
$EndComp
$Comp
L power:+7.5V #PWR03
U 1 1 5D63D35B
P 1050 2350
F 0 "#PWR03" H 1050 2200 50  0001 C CNN
F 1 "+7.5V" H 1065 2523 50  0000 C CNN
F 2 "" H 1050 2350 50  0001 C CNN
F 3 "" H 1050 2350 50  0001 C CNN
	1    1050 2350
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR04
U 1 1 5D63D361
P 1050 2550
F 0 "#PWR04" H 1050 2300 50  0001 C CNN
F 1 "GND" H 1055 2377 50  0000 C CNN
F 2 "" H 1050 2550 50  0001 C CNN
F 3 "" H 1050 2550 50  0001 C CNN
	1    1050 2550
	1    0    0    -1  
$EndComp
Text Notes 600  2450 0    50   ~ 0
Charger 2
Wire Wire Line
	1050 2550 1250 2550
$Comp
L Switch:SW_SPDT SW2
U 1 1 5D63D369
P 3550 2750
F 0 "SW2" H 3550 3035 50  0000 C CNN
F 1 "Arm Power Switch" H 3550 2944 50  0000 C CNN
F 2 "" H 3550 2750 50  0001 C CNN
F 3 "~" H 3550 2750 50  0001 C CNN
	1    3550 2750
	1    0    0    -1  
$EndComp
Wire Wire Line
	3200 2750 3350 2750
Wire Wire Line
	1050 2350 3850 2350
Wire Wire Line
	1250 3300 2500 3300
Wire Wire Line
	1250 2550 1250 3300
$Comp
L Device:Battery_Cell BT3
U 1 1 5D63D374
P 1500 2700
F 0 "BT3" H 1618 2796 50  0000 L CNN
F 1 "LiIon 18650" H 1618 2705 50  0000 L CNN
F 2 "" V 1500 2760 50  0001 C CNN
F 3 "~" V 1500 2760 50  0001 C CNN
	1    1500 2700
	1    0    0    -1  
$EndComp
$Comp
L Device:Battery_Cell BT4
U 1 1 5D63D37A
P 1500 3100
F 0 "BT4" H 1618 3196 50  0000 L CNN
F 1 "LiIon 18650" H 1618 3105 50  0000 L CNN
F 2 "" V 1500 3160 50  0001 C CNN
F 3 "~" V 1500 3160 50  0001 C CNN
	1    1500 3100
	1    0    0    -1  
$EndComp
Wire Wire Line
	1500 2800 1500 2850
Wire Wire Line
	1500 2500 2200 2500
Wire Wire Line
	1500 3200 2200 3200
Wire Wire Line
	2200 3200 2200 2950
Connection ~ 1500 2850
Wire Wire Line
	1500 2850 1500 2900
Wire Wire Line
	2200 2500 2200 2750
Wire Wire Line
	1500 2850 2200 2850
Wire Wire Line
	3750 2650 3850 2650
Wire Wire Line
	3850 2650 3850 2350
Wire Notes Line
	3900 3550 550  3550
Wire Notes Line
	550  3550 550  2050
Wire Notes Line
	550  2050 3900 2050
Wire Notes Line
	3900 2050 3900 3550
Text Notes 1850 2200 0    50   ~ 0
Arm Power Supply
$Comp
L !mrach_custom:XL4016 U1
U 1 1 5D6641AB
P 1650 4200
F 0 "U1" H 1592 4665 50  0000 C CNN
F 1 "DC-DC Step Down XL4016" H 1592 4574 50  0000 C CNN
F 2 "" H 1650 4200 50  0001 C CNN
F 3 "" H 1650 4200 50  0001 C CNN
	1    1650 4200
	-1   0    0    -1  
$EndComp
Wire Notes Line
	2650 3650 2650 7500
Wire Notes Line
	2650 7500 550  7500
Text Notes 700  3750 0    50   ~ 0
Robotic Arm
Wire Notes Line
	550  7500 550  3650
Wire Notes Line
	550  3650 2650 3650
Wire Wire Line
	5100 1650 5100 1600
Wire Wire Line
	3750 1350 4100 1350
Wire Wire Line
	4100 1350 4100 1650
Wire Wire Line
	4100 1650 5000 1650
$Comp
L Motor:Motor_DC M7
U 1 1 5D6828EB
P 4500 1200
F 0 "M7" H 4300 1550 50  0000 L CNN
F 1 "Chassis Motor 1" H 4300 1450 50  0000 L CNN
F 2 "" H 4500 1110 50  0001 C CNN
F 3 "~" H 4500 1110 50  0001 C CNN
	1    4500 1200
	1    0    0    -1  
$EndComp
$Comp
L Motor:Motor_DC M9
U 1 1 5D683653
P 6300 1200
F 0 "M9" H 6100 1550 50  0000 L CNN
F 1 "Chassis Motor 2" H 6100 1450 50  0000 L CNN
F 2 "" H 6300 1110 50  0001 C CNN
F 3 "~" H 6300 1110 50  0001 C CNN
	1    6300 1200
	1    0    0    -1  
$EndComp
Wire Wire Line
	4500 1000 4850 1000
Wire Wire Line
	5950 1000 6300 1000
$Comp
L !mrach_custom:PCA9685Board U4
U 1 1 5D690680
P 4200 4100
F 0 "U4" H 4450 4300 50  0000 L CNN
F 1 "PCA9685Board" H 3750 4300 50  0000 L CNN
F 2 "" H 4000 4100 50  0001 C CNN
F 3 "" H 4000 4100 50  0001 C CNN
	1    4200 4100
	1    0    0    -1  
$EndComp
Text Notes 3250 1400 0    50   ~ 0
7.5V
Text Notes 3300 2900 0    50   ~ 0
7.5V
Text Notes 2100 4350 0    50   ~ 0
5V
Wire Wire Line
	2900 3850 3250 3850
Wire Wire Line
	2050 4250 2550 4250
Wire Wire Line
	3750 2850 3750 3400
Wire Wire Line
	3750 3400 2800 3400
Wire Wire Line
	2800 3400 2800 4050
Wire Wire Line
	2800 4050 2050 4050
Wire Wire Line
	2050 4350 2450 4350
Text Notes 2100 4050 0    50   ~ 0
7.5V
Connection ~ 2450 4350
Wire Wire Line
	2450 4350 2450 5150
Connection ~ 2450 6750
Connection ~ 2450 6350
Connection ~ 2450 5950
Connection ~ 2450 5550
Connection ~ 2450 5150
Connection ~ 2550 4250
Connection ~ 2550 5650
Wire Wire Line
	4100 7050 4100 4500
Wire Wire Line
	4000 6650 4000 4500
Wire Wire Line
	3900 6250 3900 4500
Wire Wire Line
	3800 5850 3800 4500
Wire Wire Line
	3700 5450 3700 4500
Wire Wire Line
	3600 5050 3600 4500
Wire Wire Line
	5300 4550 5100 4550
Wire Wire Line
	5100 4550 5100 4500
Wire Wire Line
	5400 4650 5000 4650
Wire Wire Line
	5000 4650 5000 4500
Wire Wire Line
	5500 4750 4900 4750
Wire Wire Line
	4900 4750 4900 4500
$Comp
L Motor:Motor_Servo M8
U 1 1 5D6F07FE
P 5150 5550
F 0 "M8" H 5050 5250 50  0000 L CNN
F 1 "Ultrasonic Sensor Servo" H 4700 5350 50  0000 L CNN
F 2 "" H 5150 5360 50  0001 C CNN
F 3 "http://forums.parallax.com/uploads/attachments/46831/74481.png" H 5150 5360 50  0001 C CNN
	1    5150 5550
	1    0    0    -1  
$EndComp
$Comp
L LED:WS2812B D5
U 1 1 5D6F5C67
P 9450 3450
F 0 "D5" H 9550 3700 50  0000 L CNN
F 1 "WS2812B" H 9450 3800 50  0000 L CNN
F 2 "LED_SMD:LED_WS2812B_PLCC4_5.0x5.0mm_P3.2mm" H 9500 3150 50  0001 L TNN
F 3 "https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf" H 9550 3075 50  0001 L TNN
	1    9450 3450
	1    0    0    -1  
$EndComp
$Comp
L Device:Speaker LS1
U 1 1 5D6F87C5
P 10600 950
F 0 "LS1" H 10770 946 50  0000 L CNN
F 1 "Speaker" H 10770 855 50  0000 L CNN
F 2 "" H 10600 750 50  0001 C CNN
F 3 "~" H 10590 900 50  0001 C CNN
	1    10600 950 
	1    0    0    -1  
$EndComp
$Comp
L Amplifier_Audio:LM386 U14
U 1 1 5D6F975D
P 9600 1050
F 0 "U14" H 9700 1150 50  0000 L CNN
F 1 "LM386" H 9700 1250 50  0000 L CNN
F 2 "" H 9700 1150 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/lm386.pdf" H 9800 1250 50  0001 C CNN
	1    9600 1050
	1    0    0    1   
$EndComp
$Comp
L !mrach_custom:WTV020M01 U15
U 1 1 5D70675A
P 10300 1900
F 0 "U15" H 10300 2475 50  0000 C CNN
F 1 "WTV020M01" H 10300 2384 50  0000 C CNN
F 2 "" H 10150 1900 50  0001 C CNN
F 3 "" H 10150 1900 50  0001 C CNN
	1    10300 1900
	1    0    0    -1  
$EndComp
$Comp
L !mrach_custom:IR_Obstacle_Sensor U11
U 1 1 5D708F18
P 8250 1150
F 0 "U11" H 8283 1475 50  0000 C CNN
F 1 "IR Front 2" H 8283 1384 50  0000 C CNN
F 2 "" H 7950 1550 50  0001 C CNN
F 3 "" H 7950 1550 50  0001 C CNN
	1    8250 1150
	1    0    0    -1  
$EndComp
$Comp
L !mrach_custom:IR_Obstacle_Sensor U10
U 1 1 5D7099EA
P 7150 1750
F 0 "U10" H 7183 2075 50  0000 C CNN
F 1 "IR Bottom 1" H 7183 1984 50  0000 C CNN
F 2 "" H 6850 2150 50  0001 C CNN
F 3 "" H 6850 2150 50  0001 C CNN
	1    7150 1750
	1    0    0    -1  
$EndComp
$Comp
L !mrach_custom:IR_Obstacle_Sensor U12
U 1 1 5D709CE8
P 8250 1750
F 0 "U12" H 8283 2075 50  0000 C CNN
F 1 "IR Bottom 2" H 8283 1984 50  0000 C CNN
F 2 "" H 7950 2150 50  0001 C CNN
F 3 "" H 7950 2150 50  0001 C CNN
	1    8250 1750
	1    0    0    -1  
$EndComp
$Comp
L !mrach_custom:HC-SR04 U7
U 1 1 5D70E65F
P 6050 5600
F 0 "U7" H 6328 5638 50  0000 L CNN
F 1 "HC-SR04" H 6328 5547 50  0000 L CNN
F 2 "" H 6050 5600 50  0001 C CNN
F 3 "" H 6050 5600 50  0001 C CNN
	1    6050 5600
	1    0    0    -1  
$EndComp
Wire Notes Line
	11150 550  9050 550 
Wire Notes Line
	9050 550  9050 2800
Wire Notes Line
	9050 2800 11150 2800
Wire Notes Line
	11150 2800 11150 550 
Text Notes 9700 700  0    50   ~ 0
Audio Module
Wire Wire Line
	9800 1850 9250 1850
Wire Wire Line
	9250 1850 9250 1150
Wire Wire Line
	9250 1150 9300 1150
Wire Wire Line
	9150 950  9300 950 
Wire Wire Line
	9700 750  10400 750 
Wire Wire Line
	10400 750  10400 950 
Wire Wire Line
	9900 1050 10400 1050
Wire Wire Line
	6450 3000 6400 3000
Wire Wire Line
	6400 3000 6400 2750
Wire Wire Line
	6400 2750 7900 2750
Wire Wire Line
	4800 4850 4800 4500
Wire Wire Line
	4700 4500 4700 4950
Wire Wire Line
	4600 4500 4600 5050
Wire Wire Line
	4500 5450 4850 5450
Wire Wire Line
	2550 5650 4850 5650
Wire Notes Line
	4650 5200 4650 6250
Wire Notes Line
	4650 6250 6850 6250
Wire Notes Line
	6850 6250 6850 5200
Wire Notes Line
	6850 5200 4650 5200
Text Notes 5750 5300 0    50   ~ 0
Ultrasonic Distance Sensor
Connection ~ 5900 2450
Wire Wire Line
	5900 2450 8450 2450
Wire Wire Line
	3250 4150 3100 4150
Wire Wire Line
	3100 4150 3100 3600
Wire Wire Line
	3250 4050 3200 4050
Wire Wire Line
	3200 4050 3200 3700
Wire Wire Line
	3250 4250 3000 4250
Wire Wire Line
	3000 4250 3000 3500
Wire Wire Line
	3000 3500 4000 3500
Wire Wire Line
	2450 4350 3000 4350
$Comp
L power:GND #PWR08
U 1 1 5D876F82
P 3200 1800
F 0 "#PWR08" H 3200 1550 50  0001 C CNN
F 1 "GND" H 3205 1627 50  0000 C CNN
F 2 "" H 3200 1800 50  0001 C CNN
F 3 "" H 3200 1800 50  0001 C CNN
	1    3200 1800
	1    0    0    -1  
$EndComp
Connection ~ 3200 1800
$Comp
L power:GND #PWR05
U 1 1 5D881AD5
P 2500 3300
F 0 "#PWR05" H 2500 3050 50  0001 C CNN
F 1 "GND" H 2505 3127 50  0000 C CNN
F 2 "" H 2500 3300 50  0001 C CNN
F 3 "" H 2500 3300 50  0001 C CNN
	1    2500 3300
	1    0    0    -1  
$EndComp
Connection ~ 2550 6850
Connection ~ 2550 6450
Connection ~ 2550 6050
Connection ~ 2550 5250
Wire Wire Line
	2550 4250 2550 5250
Wire Wire Line
	2450 5550 4850 5550
Wire Wire Line
	2550 6850 2550 7250
Wire Wire Line
	2550 6450 2550 6850
Wire Wire Line
	2550 6050 2550 6450
Wire Wire Line
	2550 5650 2550 6050
Wire Wire Line
	2550 5250 2550 5650
Wire Wire Line
	2450 6750 2450 7150
Wire Wire Line
	2450 6350 2450 6750
Wire Wire Line
	2450 5950 2450 6350
Wire Wire Line
	2450 5550 2450 5950
Wire Wire Line
	2450 5150 2450 5550
Wire Wire Line
	1950 5050 3600 5050
Wire Wire Line
	1950 5450 3700 5450
Wire Wire Line
	1950 5850 3800 5850
Wire Wire Line
	1950 6250 3900 6250
Wire Wire Line
	1950 6650 4000 6650
Wire Wire Line
	1950 6850 2550 6850
Wire Wire Line
	1950 6450 2550 6450
Wire Wire Line
	1950 6050 2550 6050
Wire Wire Line
	1950 5650 2550 5650
Wire Wire Line
	1950 5250 2550 5250
Wire Wire Line
	2550 7250 1950 7250
Wire Wire Line
	1950 5150 2450 5150
Wire Wire Line
	1950 5550 2450 5550
Wire Wire Line
	1950 5950 2450 5950
Wire Wire Line
	1950 6350 2450 6350
Wire Wire Line
	1950 6750 2450 6750
Wire Wire Line
	2450 7150 1950 7150
$Comp
L Motor:Motor_Servo M6
U 1 1 5D640634
P 1650 7150
F 0 "M6" H 1982 7215 50  0000 L CNN
F 1 "Arm Servo 5" H 1982 7124 50  0000 L CNN
F 2 "" H 1650 6960 50  0001 C CNN
F 3 "http://forums.parallax.com/uploads/attachments/46831/74481.png" H 1650 6960 50  0001 C CNN
	1    1650 7150
	-1   0    0    -1  
$EndComp
$Comp
L Motor:Motor_Servo M5
U 1 1 5D640083
P 1650 6750
F 0 "M5" H 1982 6815 50  0000 L CNN
F 1 "Arm Servo 4" H 1982 6724 50  0000 L CNN
F 2 "" H 1650 6560 50  0001 C CNN
F 3 "http://forums.parallax.com/uploads/attachments/46831/74481.png" H 1650 6560 50  0001 C CNN
	1    1650 6750
	-1   0    0    -1  
$EndComp
$Comp
L Motor:Motor_Servo M4
U 1 1 5D63F9FE
P 1650 6350
F 0 "M4" H 1982 6415 50  0000 L CNN
F 1 "Arm Servo 3" H 1982 6324 50  0000 L CNN
F 2 "" H 1650 6160 50  0001 C CNN
F 3 "http://forums.parallax.com/uploads/attachments/46831/74481.png" H 1650 6160 50  0001 C CNN
	1    1650 6350
	-1   0    0    -1  
$EndComp
$Comp
L Motor:Motor_Servo M3
U 1 1 5D63F520
P 1650 5950
F 0 "M3" H 1982 6015 50  0000 L CNN
F 1 "Arm Servo 2" H 1982 5924 50  0000 L CNN
F 2 "" H 1650 5760 50  0001 C CNN
F 3 "http://forums.parallax.com/uploads/attachments/46831/74481.png" H 1650 5760 50  0001 C CNN
	1    1650 5950
	-1   0    0    -1  
$EndComp
$Comp
L Motor:Motor_Servo M2
U 1 1 5D63EAAF
P 1650 5550
F 0 "M2" H 1982 5615 50  0000 L CNN
F 1 "Arm Servo 1" H 1982 5524 50  0000 L CNN
F 2 "" H 1650 5360 50  0001 C CNN
F 3 "http://forums.parallax.com/uploads/attachments/46831/74481.png" H 1650 5360 50  0001 C CNN
	1    1650 5550
	-1   0    0    -1  
$EndComp
$Comp
L Motor:Motor_Servo M1
U 1 1 5D63E5E2
P 1650 5150
F 0 "M1" H 1982 5215 50  0000 L CNN
F 1 "Arm Servo 0" H 1982 5124 50  0000 L CNN
F 2 "" H 1650 4960 50  0001 C CNN
F 3 "http://forums.parallax.com/uploads/attachments/46831/74481.png" H 1650 4960 50  0001 C CNN
	1    1650 5150
	-1   0    0    -1  
$EndComp
Wire Wire Line
	1950 7050 4100 7050
$Comp
L power:GND #PWR013
U 1 1 5D8DBC9F
P 8350 4600
F 0 "#PWR013" H 8350 4350 50  0001 C CNN
F 1 "GND" H 8355 4427 50  0000 C CNN
F 2 "" H 8350 4600 50  0001 C CNN
F 3 "" H 8350 4600 50  0001 C CNN
	1    8350 4600
	1    0    0    -1  
$EndComp
Wire Wire Line
	2550 4250 2550 4150
Wire Wire Line
	2550 4150 2050 4150
$Comp
L power:GND #PWR06
U 1 1 5D9D5E7A
P 2550 7250
F 0 "#PWR06" H 2550 7000 50  0001 C CNN
F 1 "GND" H 2555 7077 50  0000 C CNN
F 2 "" H 2550 7250 50  0001 C CNN
F 3 "" H 2550 7250 50  0001 C CNN
	1    2550 7250
	1    0    0    -1  
$EndComp
Connection ~ 2550 7250
Connection ~ 2500 3300
$Comp
L power:GND #PWR010
U 1 1 5DA4378C
P 5100 1650
F 0 "#PWR010" H 5100 1400 50  0001 C CNN
F 1 "GND" H 5105 1477 50  0000 C CNN
F 2 "" H 5100 1650 50  0001 C CNN
F 3 "" H 5100 1650 50  0001 C CNN
	1    5100 1650
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR016
U 1 1 5DA99ECD
P 9800 2250
F 0 "#PWR016" H 9800 2000 50  0001 C CNN
F 1 "GND" H 9805 2077 50  0000 C CNN
F 2 "" H 9800 2250 50  0001 C CNN
F 3 "" H 9800 2250 50  0001 C CNN
	1    9800 2250
	1    0    0    -1  
$EndComp
Wire Wire Line
	9150 1950 9150 950 
Wire Wire Line
	9800 1950 9150 1950
Wire Wire Line
	8450 2450 8450 2900
Wire Wire Line
	8450 2900 8450 3200
Connection ~ 8450 2450
Wire Wire Line
	8450 2450 9350 2450
Connection ~ 8250 2750
Wire Wire Line
	8250 2750 8900 2750
Text Notes 9300 2950 0    50   ~ 0
Bottom RGB Lights
$Comp
L LED:WS2812B D7
U 1 1 5DCF6C46
P 10150 3450
F 0 "D7" H 10250 3700 50  0000 L CNN
F 1 "WS2812B" H 10150 3800 50  0000 L CNN
F 2 "LED_SMD:LED_WS2812B_PLCC4_5.0x5.0mm_P3.2mm" H 10200 3150 50  0001 L TNN
F 3 "https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf" H 10250 3075 50  0001 L TNN
	1    10150 3450
	1    0    0    -1  
$EndComp
$Comp
L LED:WS2812B D9
U 1 1 5DCF732A
P 10850 3450
F 0 "D9" H 10950 3700 50  0000 L CNN
F 1 "WS2812B" H 10850 3800 50  0000 L CNN
F 2 "LED_SMD:LED_WS2812B_PLCC4_5.0x5.0mm_P3.2mm" H 10900 3150 50  0001 L TNN
F 3 "https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf" H 10950 3075 50  0001 L TNN
	1    10850 3450
	1    0    0    -1  
$EndComp
Wire Wire Line
	8450 2900 9100 2900
Wire Wire Line
	9100 2900 9100 3150
Wire Wire Line
	9100 3150 9450 3150
Connection ~ 8450 2900
Wire Wire Line
	9450 3150 10150 3150
Connection ~ 9450 3150
Wire Wire Line
	10150 3150 10850 3150
Connection ~ 10150 3150
Wire Wire Line
	10850 3750 10600 3750
Wire Wire Line
	10150 3750 9450 3750
Connection ~ 10150 3750
$Comp
L LED:WS2812B D6
U 1 1 5DD4D33D
P 9450 4200
F 0 "D6" H 9550 4450 50  0000 L CNN
F 1 "WS2812B" H 9450 4550 50  0000 L CNN
F 2 "LED_SMD:LED_WS2812B_PLCC4_5.0x5.0mm_P3.2mm" H 9500 3900 50  0001 L TNN
F 3 "https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf" H 9550 3825 50  0001 L TNN
	1    9450 4200
	1    0    0    -1  
$EndComp
$Comp
L LED:WS2812B D8
U 1 1 5DD4D343
P 10150 4200
F 0 "D8" H 10250 4450 50  0000 L CNN
F 1 "WS2812B" H 10150 4550 50  0000 L CNN
F 2 "LED_SMD:LED_WS2812B_PLCC4_5.0x5.0mm_P3.2mm" H 10200 3900 50  0001 L TNN
F 3 "https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf" H 10250 3825 50  0001 L TNN
	1    10150 4200
	1    0    0    -1  
$EndComp
$Comp
L LED:WS2812B D10
U 1 1 5DD4D349
P 10850 4200
F 0 "D10" H 10950 4450 50  0000 L CNN
F 1 "WS2812B" H 10850 4550 50  0000 L CNN
F 2 "LED_SMD:LED_WS2812B_PLCC4_5.0x5.0mm_P3.2mm" H 10900 3900 50  0001 L TNN
F 3 "https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf" H 10950 3825 50  0001 L TNN
	1    10850 4200
	1    0    0    -1  
$EndComp
Wire Wire Line
	9100 3900 9450 3900
Wire Wire Line
	9450 3900 10150 3900
Connection ~ 9450 3900
Wire Wire Line
	10150 3900 10850 3900
Connection ~ 10150 3900
Wire Wire Line
	10150 4500 9450 4500
Connection ~ 10150 4500
Wire Wire Line
	3200 3300 3200 2950
Wire Wire Line
	4500 4500 4500 5450
$Comp
L !mrach_custom:IR_Obstacle_Sensor U9
U 1 1 5D709680
P 7150 1150
F 0 "U9" H 7183 1475 50  0000 C CNN
F 1 "IR Front 1" H 7183 1384 50  0000 C CNN
F 2 "" H 6850 1550 50  0001 C CNN
F 3 "" H 6850 1550 50  0001 C CNN
	1    7150 1150
	1    0    0    -1  
$EndComp
Wire Wire Line
	4400 4500 4400 6750
Wire Wire Line
	4300 4500 4300 7450
Wire Wire Line
	6100 5850 6100 6100
Wire Wire Line
	6000 5850 6000 6200
Wire Wire Line
	5900 5850 5900 6200
Wire Notes Line
	4650 6300 6850 6300
Wire Notes Line
	6850 6300 6850 7750
Wire Notes Line
	6850 7750 4650 7750
Wire Notes Line
	4650 7750 4650 6300
Text Notes 5650 6500 0    50   ~ 0
Headlights
$Comp
L power:GND #PWR011
U 1 1 5E0CF65E
P 6650 7500
F 0 "#PWR011" H 6650 7250 50  0001 C CNN
F 1 "GND" H 6655 7327 50  0000 C CNN
F 2 "" H 6650 7500 50  0001 C CNN
F 3 "" H 6650 7500 50  0001 C CNN
	1    6650 7500
	1    0    0    -1  
$EndComp
Wire Wire Line
	5700 7550 5850 7550
Wire Wire Line
	5850 7300 5850 7450
Wire Wire Line
	5700 7300 5850 7300
Wire Wire Line
	5700 6850 5850 6850
Wire Wire Line
	5850 6600 5850 6750
Wire Wire Line
	5700 6600 5850 6600
Wire Wire Line
	5200 7550 5400 7550
Wire Wire Line
	5400 7300 5200 7300
Wire Wire Line
	5200 6850 5200 6750
Wire Wire Line
	5400 6850 5200 6850
Wire Wire Line
	5200 6600 5400 6600
$Comp
L Device:LED D4
U 1 1 5E04C9C4
P 5550 7550
F 0 "D4" H 5700 7700 50  0000 C CNN
F 1 "LED" H 5700 7600 50  0000 C CNN
F 2 "" H 5550 7550 50  0001 C CNN
F 3 "~" H 5550 7550 50  0001 C CNN
	1    5550 7550
	-1   0    0    -1  
$EndComp
$Comp
L Device:LED D3
U 1 1 5E04C6B6
P 5550 7300
F 0 "D3" H 5700 7450 50  0000 C CNN
F 1 "LED" H 5700 7350 50  0000 C CNN
F 2 "" H 5550 7300 50  0001 C CNN
F 3 "~" H 5550 7300 50  0001 C CNN
	1    5550 7300
	-1   0    0    -1  
$EndComp
$Comp
L Device:LED D2
U 1 1 5E03753E
P 5550 6850
F 0 "D2" H 5700 7000 50  0000 C CNN
F 1 "LED" H 5700 6900 50  0000 C CNN
F 2 "" H 5550 6850 50  0001 C CNN
F 3 "~" H 5550 6850 50  0001 C CNN
	1    5550 6850
	-1   0    0    -1  
$EndComp
$Comp
L Device:LED D1
U 1 1 5D6F4429
P 5550 6600
F 0 "D1" H 5700 6750 50  0000 C CNN
F 1 "LED" H 5700 6650 50  0000 C CNN
F 2 "" H 5550 6600 50  0001 C CNN
F 3 "~" H 5550 6600 50  0001 C CNN
	1    5550 6600
	-1   0    0    -1  
$EndComp
Text Notes 5650 7200 0    50   ~ 0
Rear Lights
Wire Notes Line
	11150 2850 9050 2850
Wire Notes Line
	9050 2850 9050 4800
Wire Notes Line
	9050 4800 11150 4800
Wire Notes Line
	11150 4800 11150 2850
Wire Wire Line
	4800 4850 5600 4850
Wire Wire Line
	5600 1600 5600 4850
Wire Wire Line
	5700 1600 5700 4950
Wire Wire Line
	4700 4950 5700 4950
Wire Wire Line
	4600 5050 5800 5050
Wire Wire Line
	5000 1650 5000 1600
$Comp
L !mrach_custom:L298N_Driver_Board U6
U 1 1 5D61D823
P 4650 650
F 0 "U6" H 5400 525 50  0000 C CNN
F 1 "L298N_Driver_Board" H 5400 434 50  0000 C CNN
F 2 "" H 5300 400 50  0001 C CNN
F 3 "" H 5300 400 50  0001 C CNN
	1    4650 650 
	1    0    0    -1  
$EndComp
Wire Wire Line
	4850 1100 4800 1100
Wire Wire Line
	4800 1100 4800 1500
Wire Wire Line
	4800 1500 4500 1500
Wire Wire Line
	5300 1600 5300 4550
Wire Wire Line
	5950 1100 6000 1100
Wire Wire Line
	6000 1100 6000 1500
Wire Wire Line
	6000 1500 6300 1500
Wire Wire Line
	5400 1600 5400 4650
Wire Wire Line
	5500 1600 5500 4750
Wire Wire Line
	8250 2750 8250 3200
Wire Wire Line
	8750 3800 9000 3800
Wire Wire Line
	9000 3800 9000 4200
Wire Wire Line
	9000 4200 9150 4200
$Comp
L !mrach_custom:ESP32Board U8
U 1 1 5D61CD6D
P 7050 3850
F 0 "U8" H 7050 4250 50  0000 C CNN
F 1 "ESP32Board" H 7050 4050 50  0000 C CNB
F 2 "" H 7150 4450 50  0001 C CNN
F 3 "" H 7150 4450 50  0001 C CNN
	1    7050 3850
	1    0    0    -1  
$EndComp
Wire Wire Line
	8550 2250 8550 1850
Text Notes 7450 700  0    50   ~ 0
IR Sensors
$Comp
L power:GND #PWR015
U 1 1 5EBA9271
P 9250 650
F 0 "#PWR015" H 9250 400 50  0001 C CNN
F 1 "GND" H 9255 477 50  0000 C CNN
F 2 "" H 9250 650 50  0001 C CNN
F 3 "" H 9250 650 50  0001 C CNN
	1    9250 650 
	1    0    0    -1  
$EndComp
Wire Wire Line
	9250 650  9500 650 
Wire Wire Line
	9500 650  9500 750 
Wire Wire Line
	8900 2750 8900 1650
Wire Wire Line
	8900 1050 8550 1050
Connection ~ 8900 2750
Wire Wire Line
	8550 1650 8900 1650
Connection ~ 8900 1650
Wire Wire Line
	8900 1650 8900 1050
Wire Wire Line
	8550 1750 8800 1750
Wire Wire Line
	8800 1750 8800 1150
Wire Wire Line
	8550 1150 8800 1150
Wire Wire Line
	7450 1050 7900 1050
Wire Wire Line
	7900 1050 7900 1650
Connection ~ 7900 2750
Wire Wire Line
	7900 2750 8250 2750
Wire Wire Line
	7450 1750 7750 1750
$Comp
L power:GND #PWR014
U 1 1 5ECAAED3
P 8800 1750
F 0 "#PWR014" H 8800 1500 50  0001 C CNN
F 1 "GND" H 8805 1577 50  0000 C CNN
F 2 "" H 8800 1750 50  0001 C CNN
F 3 "" H 8800 1750 50  0001 C CNN
	1    8800 1750
	1    0    0    -1  
$EndComp
Connection ~ 8800 1750
Wire Wire Line
	7750 1750 7750 1150
$Comp
L power:GND #PWR012
U 1 1 5ECB9A33
P 7750 1750
F 0 "#PWR012" H 7750 1500 50  0001 C CNN
F 1 "GND" H 7755 1577 50  0000 C CNN
F 2 "" H 7750 1750 50  0001 C CNN
F 3 "" H 7750 1750 50  0001 C CNN
	1    7750 1750
	1    0    0    -1  
$EndComp
Connection ~ 7750 1750
Wire Wire Line
	7750 1150 7450 1150
Wire Wire Line
	7450 1650 7900 1650
Connection ~ 7900 1650
Wire Wire Line
	7900 1650 7900 2750
Wire Wire Line
	6450 3900 6000 3900
Wire Wire Line
	6000 3900 6000 2050
Wire Wire Line
	7500 2050 7500 1850
Wire Wire Line
	6000 2050 7500 2050
Wire Wire Line
	7500 1850 7450 1850
Wire Wire Line
	7600 1250 7600 2150
Wire Wire Line
	7450 1250 7600 1250
Wire Wire Line
	6450 3800 6100 3800
Wire Wire Line
	6100 3800 6100 2150
Wire Wire Line
	6100 2150 7600 2150
Wire Wire Line
	8550 1250 8650 1250
Wire Wire Line
	8650 1250 8650 2350
Wire Wire Line
	6200 2250 6200 3700
Wire Wire Line
	6200 3700 6450 3700
Wire Wire Line
	6200 2250 8550 2250
Wire Wire Line
	6300 2350 6300 3600
Wire Wire Line
	6300 3600 6450 3600
Wire Wire Line
	6300 2350 8650 2350
Wire Wire Line
	7900 3600 7950 3600
Wire Wire Line
	7800 3800 7800 3600
Wire Wire Line
	7900 3600 7900 3400
Wire Wire Line
	8750 3600 9000 3600
Wire Wire Line
	9000 3600 9000 3450
Wire Wire Line
	9000 3450 9150 3450
Wire Wire Line
	8750 4000 8900 4000
Wire Wire Line
	8750 4200 8800 4200
Wire Wire Line
	9100 3150 9100 3900
Connection ~ 9100 3150
$Comp
L power:GND #PWR018
U 1 1 5F2A3115
P 10600 4550
F 0 "#PWR018" H 10600 4300 50  0001 C CNN
F 1 "GND" H 10605 4377 50  0000 C CNN
F 2 "" H 10600 4550 50  0001 C CNN
F 3 "" H 10600 4550 50  0001 C CNN
	1    10600 4550
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR017
U 1 1 5F2A3851
P 10600 3750
F 0 "#PWR017" H 10600 3500 50  0001 C CNN
F 1 "GND" H 10605 3577 50  0000 C CNN
F 2 "" H 10600 3750 50  0001 C CNN
F 3 "" H 10600 3750 50  0001 C CNN
	1    10600 3750
	1    0    0    -1  
$EndComp
Connection ~ 10600 3750
Wire Wire Line
	10600 3750 10150 3750
Wire Notes Line
	9000 550  9000 2200
Wire Notes Line
	9000 2200 6850 2200
Wire Notes Line
	6850 2200 6850 550 
Wire Notes Line
	6850 550  9000 550 
Wire Wire Line
	6100 6100 8800 6100
Wire Wire Line
	8800 4200 8800 6100
Wire Wire Line
	8900 4000 8900 6200
Wire Wire Line
	6000 6200 8900 6200
Wire Wire Line
	5900 5150 5700 5150
Wire Wire Line
	5700 5150 5700 6200
Wire Wire Line
	5700 6200 5900 6200
Wire Wire Line
	9350 2450 9350 1350
Wire Wire Line
	9350 1350 9500 1350
Wire Wire Line
	11100 2750 11100 1550
Wire Wire Line
	11100 1550 10800 1550
Wire Wire Line
	8900 2750 11100 2750
Wire Wire Line
	7900 3400 7600 3400
Wire Wire Line
	7800 3600 7600 3600
Wire Wire Line
	7700 4000 7700 3700
Wire Wire Line
	7700 3700 7600 3700
Wire Wire Line
	7600 3000 7700 3000
Wire Wire Line
	7700 3000 7700 2550
Wire Wire Line
	7700 2550 9450 2550
Wire Wire Line
	9450 2550 9450 2150
Wire Wire Line
	9450 2150 9800 2150
Wire Wire Line
	7600 3100 7800 3100
Wire Wire Line
	7800 3100 7800 2650
Wire Wire Line
	7800 2650 11000 2650
Wire Wire Line
	11000 2650 11000 2150
Wire Wire Line
	11000 2150 10800 2150
$Comp
L Logic_LevelTranslator:TXB0102DCT U5
U 1 1 5D8FEC6A
P 4600 3050
F 0 "U5" H 5100 3700 50  0000 C CNN
F 1 "TXB0102DCT" H 4950 3600 50  0000 C CNN
F 2 "Package_SO:TSSOP-8_3x3mm_P0.65mm" H 4600 2500 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/txb0102.pdf" H 4600 3020 50  0001 C CNN
	1    4600 3050
	-1   0    0    -1  
$EndComp
$Comp
L power:GND #PWR09
U 1 1 5D9110A6
P 4600 3550
F 0 "#PWR09" H 4600 3300 50  0001 C CNN
F 1 "GND" H 4605 3377 50  0000 C CNN
F 2 "" H 4600 3550 50  0001 C CNN
F 3 "" H 4600 3550 50  0001 C CNN
	1    4600 3550
	1    0    0    -1  
$EndComp
Wire Wire Line
	5200 1600 5200 2450
Wire Wire Line
	5200 2450 5900 2450
Wire Wire Line
	4000 2450 4500 2450
Wire Wire Line
	4000 2450 4000 3500
Wire Wire Line
	3100 3600 4100 3600
Wire Wire Line
	4100 3600 4100 2950
Wire Wire Line
	4100 2950 4200 2950
Wire Wire Line
	4200 3150 4200 3700
Wire Wire Line
	4200 3700 3200 3700
Wire Wire Line
	2500 3300 3200 3300
Wire Wire Line
	2900 3850 2900 4150
Wire Wire Line
	2900 4150 2550 4150
Connection ~ 2550 4150
$Comp
L power:GND #PWR07
U 1 1 5DA1AFBC
P 2900 4150
F 0 "#PWR07" H 2900 3900 50  0001 C CNN
F 1 "GND" H 2905 3977 50  0000 C CNN
F 2 "" H 2900 4150 50  0001 C CNN
F 3 "" H 2900 4150 50  0001 C CNN
	1    2900 4150
	1    0    0    -1  
$EndComp
Connection ~ 2900 4150
Wire Wire Line
	4500 2550 4500 2450
Wire Wire Line
	6400 2750 5200 2750
Connection ~ 6400 2750
Wire Wire Line
	5200 2550 5200 2750
Wire Wire Line
	6450 3400 5200 3400
Wire Wire Line
	5200 3400 5200 2950
Wire Wire Line
	5200 2950 5000 2950
Wire Wire Line
	6450 3500 5100 3500
Wire Wire Line
	5100 3500 5100 3150
Wire Wire Line
	5100 3150 5000 3150
Wire Wire Line
	4700 2550 5200 2550
Wire Notes Line
	6800 550  6800 2200
Wire Notes Line
	6800 2200 3950 2200
Wire Notes Line
	3950 2200 3950 550 
Wire Notes Line
	3950 550  6800 550 
Text Notes 4400 700  0    50   ~ 0
Chassis Motors
Wire Wire Line
	4400 6750 5200 6750
Connection ~ 5200 6750
Wire Wire Line
	5200 6750 5200 6600
Wire Wire Line
	4300 7450 5200 7450
Wire Wire Line
	5200 7300 5200 7450
Connection ~ 5200 7450
Wire Wire Line
	5200 7450 5200 7550
Wire Wire Line
	5850 6750 6650 6750
Wire Wire Line
	6650 6750 6650 7450
Connection ~ 5850 6750
Wire Wire Line
	5850 6750 5850 6850
Wire Wire Line
	5850 7450 6650 7450
Connection ~ 5850 7450
Wire Wire Line
	5850 7450 5850 7550
Connection ~ 6650 7450
Wire Wire Line
	6650 7450 6650 7500
Wire Wire Line
	3000 4250 3000 4350
Connection ~ 3000 4250
Connection ~ 3000 4350
Wire Wire Line
	3000 4350 3250 4350
Connection ~ 5900 4600
Wire Wire Line
	5900 4600 5900 5150
Wire Wire Line
	5900 4600 5900 2450
Wire Wire Line
	5800 1600 5800 5050
Wire Wire Line
	5900 4600 6100 4600
$Comp
L Device:CP1_Small C1
U 1 1 5D808D3A
P 6100 4850
F 0 "C1" H 6191 4896 50  0000 L CNN
F 1 "10 uF" H 6191 4805 50  0000 L CNN
F 2 "" H 6100 4850 50  0001 C CNN
F 3 "~" H 6100 4850 50  0001 C CNN
	1    6100 4850
	1    0    0    -1  
$EndComp
Wire Wire Line
	6100 4750 6100 4600
$Comp
L power:GND #PWR019
U 1 1 5D81F8AC
P 6100 4950
F 0 "#PWR019" H 6100 4700 50  0001 C CNN
F 1 "GND" H 6105 4777 50  0000 C CNN
F 2 "" H 6100 4950 50  0001 C CNN
F 3 "" H 6100 4950 50  0001 C CNN
	1    6100 4950
	1    0    0    -1  
$EndComp
Wire Wire Line
	6100 4600 6350 4600
Wire Wire Line
	6350 4600 6350 4800
Wire Wire Line
	6350 4800 6450 4800
Connection ~ 6100 4600
Wire Wire Line
	7700 4000 7950 4000
Wire Wire Line
	7950 3800 7800 3800
$Comp
L Logic_LevelTranslator:TXB0104PW U13
U 1 1 5D6FF120
P 8350 3900
F 0 "U13" H 7900 3250 50  0000 C CNN
F 1 "TXB0104PW" H 8050 3150 50  0000 C CNN
F 2 "Package_SO:TSSOP-14_4.4x5mm_P0.65mm" H 8350 3150 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/txb0104.pdf" H 8460 3995 50  0001 C CNN
	1    8350 3900
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR020
U 1 1 5D931EFD
P 6200 5850
F 0 "#PWR020" H 6200 5600 50  0001 C CNN
F 1 "GND" H 6205 5677 50  0000 C CNN
F 2 "" H 6200 5850 50  0001 C CNN
F 3 "" H 6200 5850 50  0001 C CNN
	1    6200 5850
	1    0    0    -1  
$EndComp
Wire Wire Line
	10150 4500 10600 4500
Wire Wire Line
	10600 4500 10600 4550
Connection ~ 10600 4500
Wire Wire Line
	10600 4500 10850 4500
$EndSCHEMATC
