import Data.Ratio

piCalc_ :: Integer -> Ratio Integer
piCalc_ k = 
  let k8 = 8*k in
  (4%(k8+1)) - (2%(k8+4)) - (1%(k8+5)) - (1%(k8+6))

piCalc :: Integer -> Ratio Integer -> Ratio Integer -> Ratio Integer
piCalc 0 x y = piCalc_ 0 + x
piCalc n x y =  piCalc (n-1) ( x + ( piCalc_(n) / y ) ) (y / 16)

piDecs :: Integer -> Integer
piDecs n = 
  let x = piCalc (n-1) 0 (16^(n-1)) in
  ( ( numerator x ) * (10^n) ) `div` (denominator x)


