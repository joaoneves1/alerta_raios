close,1
openr,1,'raios_lidos.bin' 
stat=fstat(1)

n_blocos=(stat.size)/136

a=0ULL
b=intarr(64)

tempo=findgen(n_blocos*64)/11314.
timestamp=ulon64arr(n_blocos)
vetor_lido=intarr(n_blocos*64)

for i=0L,n_blocos-1 do begin
	readu,1,a
	timestamp[i]=a
	readu,1,b

	print,i*64,((i+1)*64)-1
	vetor_lido[i*64:((i+1)*64)-1]=b[*]
endfor

close,1
plot,vetor_lido,/xstyle,/ystyle,

end