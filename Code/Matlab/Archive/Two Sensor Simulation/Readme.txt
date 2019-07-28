This is the two sensor frequency analysis simulation. 
This algorithm was also implemented in Arduino environment.

Scan_all_with_Control.m:
Going from FirstExp to LastExp, calling Control_3.m and printing the decision result to the screen and a file, with comparison to Ground Truth.

Control_3.m:
Segment by segment processing of the input samples files. Overlapping segment length is 4096 and step size is 1024. Segments are processed by 
Child_Detect_Ener_6_freq.m and the raw decisions are going through hysteresis. 

Child_Detect_Ener_6_freq.m:
Compare energies of the child seat sensor and the reference sensor. Peaks are removed, and Hamming window is applied before calling 
lpfft_3.m that performs the frequency analysis. In version 6, sub band energies of the child sensor are compared in order to verify that significant energy exists is in the low band.

lpfft_3.m:
Frequency analysis of 4096 samples segment. In version 3, frequency sub bands energies are returned as well.

