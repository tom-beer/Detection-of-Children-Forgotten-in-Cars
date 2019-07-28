function [ wrapped_val ] = i4_wrap( val, ilo, ihi )
jlo = min(ilo,ihi);
jhi = max(ilo,ihi);
wide = jhi+1-jlo;
if wide == 1
    wrapped_val = jlo;
else
    wrapped_val = jlo + mod(val-jlo,wide);
end

