function [ y ] = daub4_transform( n, x )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
c = [0.4829629131445341 0.8365163037378079 0.2241438680420133 -0.1294095225512603];
y = x(:); 
z = zeros(1,n);
m = n;
J = zeros(1,4);
while (4 <= m)
    i=1;
    for j=0:2:(m-2)
        J(1) = i4_wrap(j,0,m-1);
        J(2) = i4_wrap(j+1,0,m-1);
        J(3) = i4_wrap(j+2,0,m-1);
        J(4) = i4_wrap(j+3,0,m-1);
        z(i) = c*y(J+1);
        z(i+0.5*m) = c(4)*y(J(1)+1)-c(3)*y(J(2)+1)+c(2)*y(J(3)+1)-c(1)*y(J(4)+1);
        i = i+1;
    end
    y(1:m) = z(1:m);
    m = m/2;     
end

