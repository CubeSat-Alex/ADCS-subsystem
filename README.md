# ADCS-subsystem responsible for Determination the Rotation angles by Gyroscope then control the payload subsytem by servo motors
## principle work :
####  Determination the Rotation angles by Gyroscope then send it in SSP packet format to OBC subsystem (raspberry pi) with SPI  protocol as Master slave communication 
####  after that OBC response with Desired Rotation angle then ADCS move servo motors until reach desired angle  
