# exam2.Rをやったあとに実行する

if(0){
postscript(file="NSDR_alone_boxplot.eps",horiz=F,height=10,width=8,pointsize=20, family="Times")
par(mai=c(1.5,1.5,0.5,0.5),omi=c(0,0,0,0))
boxplot(t(NSDR_alone[5:1,]), xlab="input SNR [dB]", ylab="NSDR [dB]", names=-2:2*5)
dev.off();

postscript(file="NSDR_HPF_boxplot.eps",horiz=F,height=10,width=8,pointsize=20, family="Times")
par(mai=c(1.5,1.5,0.5,0.5),omi=c(0,0,0,0))
boxplot(t(NSDR_HPF[5:1,]), xlab="input SNR [dB]", ylab="NSDR [dB]", names=-2:2*5)
dev.off();
}


# 曲の長さを測る
if(0){
  weight <- c(0);
  i <- 1;
  for(file in fs1){
  	tmp <- readWave(sprintf("./MIR-1K/UndividedWavfile/%s",file));
  	weight[i] <- length(tmp@left);
    i <- i + 1;
  }
}

# GNSDR
GNSDR <- function(NSDR, weight){
  return(sum(NSDR * weight) / sum(weight))
}

ours1 <- c(GNSDR(NSDR_alone[4,], weight), 
          GNSDR(NSDR_alone[3,], weight), 
          GNSDR(NSDR_alone[2,], weight))
ours2 <- c(GNSDR(NSDR_HPF[4,], weight), 
          GNSDR(NSDR_HPF[3,], weight), 
          GNSDR(NSDR_HPF[2,], weight))

# 従来法のGNSDRのプロット, 値は論文から読み取ったもの
hsu <- c(-0.5, 0.9, 0.2)
ozerov <- c(0.52, -0.7, -3.2)
li <- c(-1.2, 0.3, 0)
rafii <- c(0.52, 1.11, 1.10)

dB <- c(-5, 0, 5)

#
x <- matrix(c(ours2,
              ozerov,
              li,
              hsu,
              rafii),3)
postscript(file="compare.eps",horiz=F,onefile=F,height=10,width=8,pointsize=20,family="Times")
  par(mai=c(1.5,1.5,0.5,0.5),omi=c(0,0,0,0))
  matplot(dB, x,
         type = "o", col = 1,
         xlim=c(-6,6),
         xaxp  = c(-5, 5, 2),
         ylim = c(-4, 5),
         yaxp  = c(-4, 5, 9),
         xlab = "Input SNR [dB]",
         ylab = "GNSDR [dB]",
         pch = c(1,2,3,5,8))
  legend(-5.7, -1.7,
         c("Proposed Method",
           "Ozerov [3]",
           "Li [4]",
           "Hsu [5]",
           "Rafii [6]"), pch= c(1,2,3,5,8), pt.bg="white", lty=0, col = "black")
dev.off()


