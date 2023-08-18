Simulate palpable heart beat pulses on a mannequin for medical training purposes.  

## Introduction  
This document will serve as a manual for operating and maintaining the pulse simulator device developed by The University of Akron students Andrew Soltisz, Emily O’Brien, Gregory Riff, and Olivia Petrey. This document was authored by Andrew Soltisz whom you can contact at (phone) 330-569-4138 or (email) andysoltisz@gmail.com. The pulse simulator device was developed in Dr. James A. Keszenheimer’s (jkeszenheimer@uakron.edu) Biomedical Engineering Senior Design course during the Fall 2016 and Spring 2017 semesters at the University of Akron. This device was built for the design team’s client, Dr. Paul Lecat (plecat4@gmail.com).  

## Background  
The purpose of this project is to optimize the cardiological education of medical students by providing a more realistic training experience.  Simulating the various types of heart disease on a mannequin will permit the students to practice taking vitals in the proper anatomical locations and distinguish the difference between the pulsations of a healthy heart versus a diseased heart.  Many of the students at NEOMED who will utilize this simulation device plan to specialize in family or general medicine.  Therefore, the focus of this device is to teach these students how to detect the early stages of heart disease during normal check-up routines, increasing the number of patients referred to a cardiologist before the disease reaches a deadly state.  

Circulatory pulses are simulated in three different locations on the body: the thoracic cavity from the apical heartbeat, the carotid artery in the neck, and the jugular vein in the neck (**Fig. 1**). Because the apical and carotid pulses are not visible and only palpable, eccentric rotating mass (ERM) vibrating motors were used to generate the heart beat in these locations. The jugular vein can only be seen so linearly actuating solenoids were used to generate these pulses. Originally, our team wanted to have the pulses at the jugular vein react to the oriented of the mannequin which is why three solenoids were used at this site. The plan was to have only one active solenoid while the mannequin was in a supine position, and as the mannequin became more upright, additional solenoids would become active with all three solenoids being active with the mannequin in a full upright position. Our team ran out of time to implement this feature, however. In the mannequin’s current state, all three solenoids are always active and fire in unison. An Arduino Uno is used to control the operations of the ERM motors and solenoids, as well as process inputs from the user (**Fig. 2**). The user has the ability to control the heart rate of the pulses using a potentiometer, the type of pulse pattern with a sliding switch, and the user can toggle the power to the solenoids using a rocker switch. Four groups of three parallel AA batteries are wired in series to power all electronic hardware. The ERM vibrating motors are controlled by a Darlington transistor array (ULN2003) and pulse width modulation (PWM) signal from the Arduino. The solenoids are controlled by a transistor relay which is toggled by a digital high or low signal from the Arduino. Each transistor relay is comprised of a 1kΩ resistor, 1N4004 diode, and a TIP120 transistor. All circuitry is wired into breadboards for ease of prototyping and modularity.  

![Picture1](https://github.com/andrewsoltisz/PulseSimulator/assets/54912641/51cf041a-c82b-400c-b7eb-7ad3e8cc5530)  
**Figure 1**: Picture of the mannequin with all three of the pulse locations labeled. ERM motors were used to generate palpable pulses at the carotid artery and apical sites while solenoids were used to generate visual pulses at the jugular vein site.  

  
![Circuit Schematic](https://github.com/andrewsoltisz/PulseSimulator/assets/54912641/fc9ed34c-ca24-4293-a136-238ecf2d5792)  
**Figure 2**: Circuit diagram of the control system for generating heart beats in the simulation mannequin.  ERM = Eccentric Rotating Mass, SOL = Solenoid, POW = Power, POT = Potentiometer, SWCH = Switch, NEO = Neopixel.

## Operation Instructions  
In its current state, the pulse simulator device is incomplete but operational. In this state, the Arduino and solenoids must be powered from separate voltage sources to avoid back current which can damage the Arduino. To this end, the Arduino can be powered through either the USB or power jack while the solenoid circuit must be powered by a separate power source such as the on-board batteries or an external power supply. Use the included circuit diagram to determine where to provide power for the solenoids. The Arduino and solenoids both require 5-12V to operate. At these voltages, the Arduino will draw only a few hundred mA while the solenoids will collectively draw 3.3A. The ERM motors are powered by the Arduino so no extra work is required to operate them. Once the Arduino and solenoids are powered by separate sources, the mannequin should be fully operational so long as the code is still installed on the Arduino’s chip. 

If the pulse simulator is fully functional, use the control box that is hard-wired to the mannequin to control the device (**Fig. 3**). When the mannequin is first started, it will be in a standby state where the neopixel LED will not be lit and the solenoids and ERM motors will not be active. The slide switch can be used to cycle through different pulse patterns of which there are 4. The first state simulates a healthy pulse pattern that will cause the neopixel LED to light up green; the second state simulates tricuspid regurgitation where the LED will be lit red; the third state simulates tricuspid stenosis where the LED will be lit blue; and the fourth and final state simulates full AV block where the LED will be lit purple. If the slide switch is switched for a fifth time after simulating full AV block, the mannequin will return to the stand-by state where all pulse patterns can be cycled through again using the slide switch. Use the rocker switch to control the power to the solenoids. When the rocker switch is on the OFF position, the solenoids will receive no power and will therefore not be active. When the rocker switch is on the ON position, the solenoids will receive power, and if the mannequin is in a pulse simulation state, the solenoids will be active. Use the potentiometer dial to change the heart rate of the pulse pattern simulations. The lowest heart rate is 80bmp when the potentiometer is turned full-counterclockwise. The highest heart rate is 220bmp when the potentiometer is turned full-clockwise. The wires for each of the components in the control box are label on the back of the control box for troubleshooting purposes.  

![Picture2](https://github.com/andrewsoltisz/PulseSimulator/assets/54912641/458377d6-d04e-4432-85ae-6bc10c26ab5d)  
**Figure 3**: Picture of the control box used to operate the mannequin. The rocker switch controls the power to the solenoids; the potentiometer controls the heart rate of the pulse simulators; the slide switch cycles through the pulse patterns every time the switch changes state; and the neopixel LED changes color according to the pulse pattern being simulated by the mannequin. No light = standby/off, green = healthy pulse, red = tricuspid regurgitation, blue = tricuspid stenosis, and purple = full AV block. The wires to each of the components are labeled on the back of the control box.
