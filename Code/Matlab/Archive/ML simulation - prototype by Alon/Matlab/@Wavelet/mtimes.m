function res = mtimes(a,b)

if isa(a,'Wavelet') == 0
    error('In  A.*B only A can be Wavelet operator');
end

if numel(b)~=max(size(b))
if a.adjoint
    res = IWT2_PO(real(b),a.wavScale,a.qmf) + i*IWT2_PO(imag(b),a.wavScale,a.qmf);
else
    res = FWT2_PO(real(b),a.wavScale,a.qmf) + i* FWT2_PO(imag(b),a.wavScale,a.qmf);
end
else
    if a.adjoint
    res = IWT_PO(real(b),a.wavScale,a.qmf) + i*IWT_PO(imag(b),a.wavScale,a.qmf);
else
    res = FWT_PO(real(b),a.wavScale,a.qmf) + i* FWT_PO(imag(b),a.wavScale,a.qmf);
    end
end



