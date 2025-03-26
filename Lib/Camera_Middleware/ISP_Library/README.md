# STM32 ISP Library

version 0.3.0

ISP Library middleware (running on the target) hosting 2A algorithms
(Auto Exposure and Auto White Blance) and mechanisms to control the
ISP and load sensor tuning file.

## Structure
- isp: core of the ISP Library with the ISP parameter configuration
- isp_param_conf: collection of sensor tuning parameters
- evision: 2A algorithms that are deliveres as binary

## Known Issues and Limitations
- When transitioning from a dark to a bright scene, a black frame can be seen during Auto Exposure (AE) algorithm convergence\*
- In rare situations, Auto White Balance (AWB) algorithm can flicker between 2 AWB profiles\*
<br>
* Fix is under preparation
