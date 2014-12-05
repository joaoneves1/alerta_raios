close,1
openr,1,'raios_detetados.bin' 

a=0L
b=intarr(13577)

tempo=findgen(13577)/11314.

timestamp=lonarr(n)
vetor_disco=intarr(13577)

readu,1,timestamp
readu,1,vetor_disco
; for i=0,n-1 do begin
; 	readu,1,a
; 	timestamp[i]=a
; 	readu,1,b
; 	vetor_disco[i,*]=b[*]
; endfor

close,1
plot,tempo,vetor_disco,/xstyle

end