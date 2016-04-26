from sense_hat import SenseHat
import time, math, argparse

def rgb(C,X,H):
  if H<60:
    return (C,X,0)
  elif H<120:
    return (X,C,0)
  elif H<180:
    return (0,C,X)
  elif H<240:
    return (0,X,C)
  elif H<300:
    return (X,0,C)
  else:
    return (C,0,X)

def C(v,s):
  return v*s

def X(c,h):
  return c*(1- abs((h/60)%2-1))

def M(v,c):
  return v-c

def HSV_to_RGB(h,s,v):
  c=C(v,s)
  x=X(c,h)
  m=M(v,c)
  (r2,g2,b2)=rgb(c,x,h)
  r=(r2+m)*255
  g=(g2+m)*255
  b=(b2+m)*255
  return (r,g,b)

def XY_to_HSV(x,y,v=0.5):
  pomak_x=4
  pomak_y=4
  x-=pomak_x
  y-=pomak_y
  s=math.pow(x,2)+math.pow(y,2)
  s=math.sqrt(s)/math.sqrt(32)
  h=math.atan2(y,x)
  h=math.degrees(h)+180
  return (h,s,v)

def main(v):
	if v is None:
		v=0.5
	else:
		v=float(v)
	sense=SenseHat()
	for i in range(8):
 		for j in range(8):
    			(h,s,v)=XY_to_HSV(i,j,v)
    			(r,g,b)=HSV_to_RGB(h,s,v)
    			#print (i,j,int(r),int(g),int(b),int(h),int(s),int(v))
    			sense.set_pixel(i,j,int(r),int(g),int(b))


if __name__ == "__main__":
	parser = argparse.ArgumentParser()
	parser.add_argument('-v')
	args = parser.parse_args()
	main(args.v)
  
