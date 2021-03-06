# Large Scale Experiment

source("./Rlib/mylib.R");

HPSS_done <- 0; # 0: not done, 1: done

len <- 512;
shi <- 256;

# volume is not adjusted, and signal and noise are known
SNR1 <- function(sig, noise){
	pow_sig <- sum(sig**2);
	pow_noise <- sum(noise**2);
	snr <- 10 * log(pow_sig/pow_noise)/log(10);
	return(snr);
}

# Volume is adjusted
SNR2 <- function(mixed, sig){
	# x(t) = a * s(t) + n(t), multiply s, expectation value, assuming that sum(sn)=0
	a <- sum(1.0 * mixed * sig)/sum(1.0 * sig**2);
	noise <- mixed - a * sig;
	snr <- SNR1(a * sig, noise);
	return(snr);
}

SDR <- function(x, y){
	x <- x / 1000;
	y <- y / 1000;
	s <- sum(x * y)**2;
	a <- s / (sum(x**2) * sum(y**2) - s)
	sdr <- 10 * log(a) / log(10);
	return (sdr);
}

# Normalized SDR
NSDR <- function(n,m,x){
	return(SDR(n,m) - SDR(x,m));
}

# Mix signal and noise in an arbitrary SNR.
mix <- function(signal, noise, db){
	return (signal + noise * exp(-db/20*log(10)) ) 
}

######################################################################
HPSS <- function(data, frame, q, w){
	# concatenate noise both side of the signal
	range <- frame + 1:length(data);
	data <- c(runif(1:frame)* 0.01, data, runif(1:frame)* 0.01);
	out <- Wave(data, samp.rate=16000, bit=16);
	writeWave(out,"./tmp/_input.wav");

	cmdline <- paste("./slidingHPSS --input ./tmp/_input.wav --block 100 --frame ", frame, 
		 " --Hout ./tmp/_H.wav --Pout ./tmp/_P.wav -Q ", q , " -W ", w, sep="");
	system(cmdline);
	print(cmdline);

	tmp <- readWave("./tmp/_H.wav"); H <- tmp@left[range];
	tmp <- readWave("./tmp/_P.wav"); P <- tmp@left[range];
	return(list(H=H, P=P));
}

# Multi-stage HPSS
MSHPSS <- function(data, frame1, frame2){
	hp1 <- HPSS(data, frame1, 0.3, 0.3);
	H1 <- hp1$H;
	P1 <- hp1$P;

	hp2 <- HPSS(H1, frame2, 0.3, 0.3);
	H2 <- hp2$H;
	P2 <- hp2$P;

	# High Pass Filter
	tmp <- stft.half(c(runif(1:512), P2, runif(1:512)),512, 256);
	tmp[,16000/512*(1:257-1) < 110] <- 0;
	Filtered <- istft.half(tmp, 256)[512+ 1:length(P2)]

	return(list(V=P2, F=Filtered, NV=P1+H2));
}

#######################################################################

# Protocol:
#  List files
#  Mix Left and Right channels.
#    Concatenate short noise both side of the signal
#  Apply HPSS
#     W-V is the vocal off
#  SNR
#  Statistics
#    (frame-wise), file-wise, (singer-wise)

fs1 <- list.files("./MIR-1K_for_MIREX/Wavfile/")

NSDR_alone <- matrix(0, 5, length(fs1))
NSDR_HPF <-   matrix(0, 5, length(fs1))

i = 0;
for(file in fs1){
	i <- i + 1;
	print(sprintf("Files: %d / %d", i, length(fs1)));

	print(file);
	tmp <- readWave(sprintf("./MIR-1K_for_MIREX/Wavfile/%s",file));
	accomp <- tmp@left;
	singer <- tmp@right;
	input_p10 <- accomp + singer * 10**0.5; 	# 10dB
	input_p5  <- accomp + singer * 10**0.25; 	#  5dB
	input_0   <- accomp + singer;            	#  0dB
	input_m5  <- accomp + singer / 10**0.25; 	# -5dB
	input_m10 <- accomp + singer / 10**0.5; 	#-10dB

	print(sprintf("SNR (singer to accompaniment) = %f [dB]", SNR1(singer,accomp)));
	print(sprintf("SNR (singer to accompaniment) = %f [dB]", SNR2(input_0,singer)));
	print(sprintf("SNR (input_m5 to singer) = %f [dB]",      SNR2(input_m5,singer)));

	out_p10 <- normalize(Wave(input_p10, samp.rate=16000, bit=16), unit="16") * 0.1;
	out_p5  <- normalize(Wave(input_p5, samp.rate=16000, bit=16), unit="16") * 0.1;
	out_0   <- normalize(Wave(input_0,  samp.rate=16000, bit=16), unit="16") * 0.1;
	out_m5  <- normalize(Wave(input_m5, samp.rate=16000, bit=16), unit="16") * 0.1;
	out_m10  <- normalize(Wave(input_m10, samp.rate=16000, bit=16), unit="16") * 0.1;

	writeWave(out_p10 * 9, sprintf("./tmp/p10_%s", file));
	writeWave(out_p5 * 9,  sprintf("./tmp/p5_%s", file));
	writeWave(out_0 * 9,   sprintf("./tmp/0_%s", file));
	writeWave(out_m5 * 9,  sprintf("./tmp/m5_%s", file));
	writeWave(out_m10 * 9, sprintf("./tmp/m10_%s", file));

	# apply HPSS
	if(!HPSS_done){
		short <- 128;
		long <- 8192;

		VNV_p10 <- MSHPSS(out_p10@left, short,long);
		VNV_p5 <-  MSHPSS(out_p5@left, short,long);
		VNV_0 <-   MSHPSS(out_0@left,  short,long);
		VNV_m5 <-  MSHPSS(out_m5@left, short,long);
		VNV_m10 <- MSHPSS(out_m10@left, short,long);

		writeWave(normalize(Wave(VNV_p10$V, samp.rate=16000, bit=16),unit="16") * 0.9, sprintf("./tmp/Vp10_%s", file));
		writeWave(normalize(Wave(VNV_p5$V,  samp.rate=16000, bit=16),unit="16") * 0.9, sprintf("./tmp/Vp5_%s", file));
		writeWave(normalize(Wave(VNV_0$V,   samp.rate=16000, bit=16),unit="16") * 0.9, sprintf("./tmp/V0_%s", file));
		writeWave(normalize(Wave(VNV_m5$V,  samp.rate=16000, bit=16),unit="16") * 0.9, sprintf("./tmp/Vm5_%s", file));
		writeWave(normalize(Wave(VNV_m10$V, samp.rate=16000, bit=16),unit="16") * 0.9, sprintf("./tmp/Vm10_%s", file));

		writeWave(normalize(Wave(VNV_p10$F, samp.rate=16000, bit=16),unit="16") * 0.9, sprintf("./tmp/Fp10_%s", file));
		writeWave(normalize(Wave(VNV_p5$F,  samp.rate=16000, bit=16),unit="16") * 0.9, sprintf("./tmp/Fp5_%s", file));
		writeWave(normalize(Wave(VNV_0$F,   samp.rate=16000, bit=16),unit="16") * 0.9, sprintf("./tmp/F0_%s", file));
		writeWave(normalize(Wave(VNV_m5$F,  samp.rate=16000, bit=16),unit="16") * 0.9, sprintf("./tmp/Fm5_%s", file));
		writeWave(normalize(Wave(VNV_m10$F, samp.rate=16000, bit=16),unit="16") * 0.9, sprintf("./tmp/Fm10_%s", file));

		writeWave(normalize(Wave(VNV_p10$NV, samp.rate=16000, bit=16),unit="16") * 0.9, sprintf("./tmp/NVp10_%s", file));
		writeWave(normalize(Wave(VNV_p5$NV,  samp.rate=16000, bit=16),unit="16") * 0.9, sprintf("./tmp/NVp5_%s", file));
		writeWave(normalize(Wave(VNV_0$NV,   samp.rate=16000, bit=16),unit="16") * 0.9, sprintf("./tmp/NV0_%s", file));
		writeWave(normalize(Wave(VNV_m5$NV,  samp.rate=16000, bit=16),unit="16") * 0.9, sprintf("./tmp/NVm5_%s", file));
		writeWave(normalize(Wave(VNV_m10$NV, samp.rate=16000, bit=16),unit="16") * 0.9, sprintf("./tmp/NVm10_%s", file));

	}else{
		VNV_p10$V <- readWave(sprintf("./tmp/Vp10_%s", file))@left;
		VNV_p5$V <-  readWave(sprintf("./tmp/Vp5_%s",  file))@left;
		VNV_0$V <-   readWave(sprintf("./tmp/V0_%s",   file))@left;
		VNV_m5$V <-  readWave(sprintf("./tmp/Vm5_%s",  file))@left;
		VNV_m10$V <- readWave(sprintf("./tmp/Vm10_%s", file))@left;
		VNV_p10$F <- readWave(sprintf("./tmp/Fp10_%s", file))@left;
		VNV_p5$F <-  readWave(sprintf("./tmp/Fp5_%s",  file))@left;
		VNV_0$F <-   readWave(sprintf("./tmp/F0_%s",   file))@left;
		VNV_m5$F <-  readWave(sprintf("./tmp/Fm5_%s",  file))@left;
		VNV_m10$F <- readWave(sprintf("./tmp/Fm10_%s", file))@left;
		VNV_p10$NV <- readWave(sprintf("./tmp/NVp10_%s", file))@left;
		VNV_p5$NV <-  readWave(sprintf("./tmp/NVp5_%s",  file))@left;
		VNV_0$NV <-   readWave(sprintf("./tmp/NV0_%s",   file))@left;
		VNV_m5$NV <-  readWave(sprintf("./tmp/NVm5_%s",  file))@left;
		VNV_m10$NV <- readWave(sprintf("./tmp/NVm10_%s", file))@left;
	}

	# SDR
	NSDR_alone[1,i] = NSDR(VNV_p10$V, singer, out_p10@left)
	NSDR_alone[2,i] = NSDR(VNV_p5$V,  singer, out_p5@left)
	NSDR_alone[3,i] = NSDR(VNV_0$V,   singer, out_0@left)
	NSDR_alone[4,i] = NSDR(VNV_m5$V,  singer, out_m5@left)
	NSDR_alone[5,i] = NSDR(VNV_m10$V, singer, out_m10@left)
	print(sprintf("SDR (+10dB) = %f [dB]",NSDR_alone[1,i]));
	print(sprintf("SDR (+5dB)  = %f [dB]",NSDR_alone[2,i]));
	print(sprintf("SDR ( 0dB)  = %f [dB]",NSDR_alone[3,i]));
	print(sprintf("SDR (-5dB)  = %f [dB]",NSDR_alone[4,i]));
	print(sprintf("SDR (-10dB) = %f [dB]",NSDR_alone[5,i]));

	NSDR_HPF[1,i] = NSDR(VNV_p10$F, singer, out_p10@left)
	NSDR_HPF[2,i] = NSDR(VNV_p5$F,  singer, out_p5@left)
	NSDR_HPF[3,i] = NSDR(VNV_0$F,   singer, out_0@left)
	NSDR_HPF[4,i] = NSDR(VNV_m5$F,  singer, out_m5@left)
	NSDR_HPF[5,i] = NSDR(VNV_m10$F, singer, out_m10@left)
	print(sprintf("SDR (+10dB) = %f [dB]",NSDR_HPF[1,i]));
	print(sprintf("SDR (+5dB)  = %f [dB]",NSDR_HPF[2,i]));
	print(sprintf("SDR ( 0dB)  = %f [dB]",NSDR_HPF[3,i]));
	print(sprintf("SDR (-5dB)  = %f [dB]",NSDR_HPF[4,i]));
	print(sprintf("SDR (-10dB) = %f [dB]",NSDR_HPF[5,i]));

}


