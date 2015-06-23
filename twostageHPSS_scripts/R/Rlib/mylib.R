library(tuneR);

"%+=%" <- function(a,b) eval.parent(substitute(a <- a + b));
"%-=%" <- function(a,b) eval.parent(substitute(a <- a - b));
"%*=%" <- function(a,b) eval.parent(substitute(a <- a * b));
"%/=%" <- function(a,b) eval.parent(substitute(a <- a / b));

ifft <- function(x) (fft(x, inverse=TRUE)/length(x));

fft.half <- function(x)
	 (fft(x)[1:(length(x)/2+1)]);
ifft.half <- function(x)
	  (Re(fft(
	  	  c(x, Conj(rev(x[2:(length(x)-1)]))),
		  inverse=TRUE)/(length(x)*2-2)));

hamming <- function(x) (0.54 - 0.46 * cos(2 * pi * x / length(x)));
hanning <- function(x) (0.50 - 0.50 * cos(2 * pi * x / length(x)));
sqrthan <- function(x) (sin(pi * x/length(x)));

stft.half <- function(wave, len, shi, winfunc=function(x){sqrt(hanning(x))}){
  n.frame <- length(wave) %/% shi - 1;
  nyq <- len/2 + 1;
  stft <- matrix(nrow = n.frame, ncol = nyq);
  for(i in 1:(n.frame)){
    stft[i,] <- fft.half(wave[(i-1) * shi + (1:len)] * winfunc(1:len));
  }
  return(stft);
}

istft.half <- function(STFT, shi,winfunc=function(x){sqrt(hanning(x))}){
  len <- length(STFT[1,]) * 2 - 2;
  n.frame <- length(STFT[,1]);
  win <- winfunc(1:len);
  invSTFT <- numeric((n.frame+1) * shi);
  for(i in 1:(n.frame)){
    tmp <- Re(win * ifft.half(STFT[i,]));
    invSTFT[(shi * (i-1) + 1):(shi * (i-1) + len)] %+=% tmp[1:len];
  }
  return(invSTFT);
}

wavsave <- function(wave, file){
  out <- normalize(Wave(wave, samp.rate=8000, bit=16), unit="16") * 0.1;
  writeWave(out,file);
}

rbcol <- rainbow(100,start=0,end=0.7);
#rbcol <- rainbow(100,start=0,end=0.75);
mncol <- gray(1:100/100);

#powscale <- function(x){log(abs(x))}
powscale <- function(x){abs(x)**0.5}

spectrogram <- function(wave){
  len <- 256; shi <- len/2;
  tmp <- stft.half(wave, len,shi);
	image(1:length(tmp[,1])*shi/8000, 1:length(tmp[1,])/shi*4000,-powscale(tmp),col=rbcol,#mncol,#rbcol,
	     xlab = "Time [s]", ylab="Frequency [Hz]"); #サンプリングが8000を想定
}

