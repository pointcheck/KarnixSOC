TARGETS += USBMain.png USBMain.pdf
TARGETS += USBSendReset.png USBSendReset.pdf
TARGETS += USBKeepAlive.png USBKeepAlive.pdf
TARGETS += USBSendShortToken.png USBSendShortToken.pdf
TARGETS += USBSendLongToken.png USBSendLongToken.pdf
TARGETS += USBSendData.png USBSendData.pdf
TARGETS += USBReceiver1.png USBReceiver1.pdf
TARGETS += USBReceiver2.png USBReceiver2.pdf
TARGETS += USBReceiver3.png USBReceiver3.pdf
TARGETS += USBReceiver.png USBReceiver.pdf

all: $(TARGETS)

%.dot:	%.m4
	m4 $^ > $@ 

%.png:	%.dot
	dot -Tpng $^ -o $@ 

%.pdf:	%.dot
	dot -Tpdf $^ -o $@

clean:
	rm $(TARGETS)


