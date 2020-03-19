# dcon
Program for data acquisition and logged from devices communication via DCON protocol
(e.g. modules ADAM 4000 serie from Adwantech or I-7000 modules from ICP CON).
Program is writen in unix enviroments (tested in Linux and Open BSD system)
and included simple terminal for module setting and tests.
Program allows transformation measuremets values via tables defined in file
'calibration.dat' (Lagrangian interpolation polynom).


iData acqusition modules ommunicate with computer via serial interface,
usually RS485 interface.
E.g. for allowed USB/serial adapter interfeace for user John type command:
   sudo usermod -a -G dialout John
Then user John can read/write serials (USB) ports.
