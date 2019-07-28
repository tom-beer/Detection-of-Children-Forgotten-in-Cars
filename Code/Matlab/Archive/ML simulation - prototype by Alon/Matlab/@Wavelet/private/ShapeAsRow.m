function row = ShapeAsRow(sig)
% ShapeAsRow -- Make signal a row vector
%  Usage
%    row = ShapeAsRow(sig)
%  Inputs
%    sig     a row or column vector
%  Outputs
%    row     a row vector
%
%  See Also
%    ShapeLike
%
row = sig(:)';    