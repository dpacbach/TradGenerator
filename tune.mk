
tune: tune.tar

play: tune
	#mpg123 --quiet --buffer 1024 --loop 2 tune.mp3 
	mpg123 --buffer 1024 tune.mp3 

clean:
	rm tune.abc tune.ps tune.pdf tune.mid tune.wav tune.mp3 tune.tar

tune.tar: tune.abc tune.mp3 tune.pdf
	tar -cvf tune.tar tune.abc tune.mp3 tune.pdf

tune.mp3: tune.wav
	ffmpeg -y -i tune.wav tune.mp3

tune.wav: tune.mid
	timidity tune.mid -Ow

tune.mid: tune.abc
	abc2midi tune.abc
	ls | grep "tune[2-9].mid" | xargs rm
	mv tune1.mid tune.mid

tune.pdf: tune.ps
	ps2pdf tune.ps tune.pdf

tune.ps: tune.abc
	abcm2ps tune.abc -O tune.ps

tune.abc:
	./tradgen

.PHONY: tune play clean
