------------------------------------------------------------------------------------

Használati útmutató:
  A program fordításához szükséges OpenGL(http://cg.elte.hu/~bsc_cg/resources/)
                                és Qt(http://www.qt.io/download-open-source/#section-2).
         
  A felhasználói felület segítségével elvághatjuk a bekódolt síkkal a kockánkat, majd a
  kapott eredmény közül ki tudjuk választani a számunkra megfelelőt.
  Ezt megismételhetjük restart segítségével.  
  FONTOS: MÉG ne vágjunk 2x egymás után (error).
                                
TODO:
  -  shader átlátszóság : készítettem saját sortert ->működik de nlogn es futásidejű
  -  Kaptam 2 bugot
      - egyiket hackel levédtem, requestet módítok acceptnél gl es részen, így nem jön a sorterből error
      - a sík rosszul rajzolódik ki ha y,z normálisokat módosítjuk
      
-------------------------------------------------------------------------------------

Funkcióbillentyűk:
  - + és - (numpad) : atomok közötti váltás, az aktív atom nem átlátszó
  - t : átlátszóság ki-be kapcsolása
  - p : 3D és 2D nézeti kapcsoló
  - i : egyéb információk lekérése konzolra
  - wasd + mouse left press : kamera mozgatása
  - nyilak : fény mozgatása

Megj.:
  A +/- funkc. billentyű nem működik ha vágó módban vagyunk, 
  ekkor a legördülő menükkel tudjuk váltani a vágandó atom részeit(pozitív, negatív, mind2)
