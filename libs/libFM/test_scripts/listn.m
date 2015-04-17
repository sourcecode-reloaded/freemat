a = shnd(9)
addlistener(a,'trigger',@(x,y) print(y))
notify(a,'trigger')
