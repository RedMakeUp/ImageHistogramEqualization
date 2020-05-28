# ImageHistogramEqualization
Import an image, then perform grayscale and finally equalize its histogram
<br/>
Test on Windows10, but not rely on it in theory<br/>
Compiled on MinGw 7.3.0 64-bit<br/>
Require C++11 and Qt 5.4 later<br/>

# Note
1.The core algorithms for image processing is in *ImageDealer.h/cpp*, other files are used to interact with GUI(Qt).<br/>
2.*stb_image* is used to decode the image into an consecutive array of pixels(or raw data). *ImageDealer.h/cpp* only knows the pixel array, not any class in Qt.<br/>
3.*qcustomplot* is a plugin for plotting curve. If there is any issue with its DLL, please recompile its source in folder **qcustomplot-source**<br/>

# Showcase
![UI](Showcases/SecondUI.png)
![gray](Showcases/gray.png)
![Mirror-Y](Showcases/Mirror-Y.png)
![Equalize](Showcases/Equalize.png)
![5x5_weight_by_5_times](Showcases/5x5_weight_by_5_times.png)
![5_max](Showcases/5_max.png)
![3x3_laplace](Showcases/3x3_laplace.png)
