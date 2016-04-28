------------------------------------------------------------------------------------

Használati útmutató:
  A program fordításához szükséges OpenGL(http://cg.elte.hu/~bsc_cg/resources/)
                                és Qt(http://www.qt.io/download-open-source/#section-2).
         
  A felhasználói felület segítségével elvághatjuk a bekódolt síkkal a kockánkat, majd a
  kapott eredmény közül ki tudjuk választani a számunkra megfelelőt.
  Ezt megismételhetjük restart segítségével.  
                                
TODO:
  - Shader fejlesztés doksi alapján
  - Testing (fourier-coefficient)
      
-------------------------------------------------------------------------------------

Functionbuttons:
  - p : switch between 3D and 2D(projection)
  - wasd + mouse left press : camera moving
  - left,right etc : light moving on sphere surface
  - o : (only 2D) switch between positive and negative projection
  - numpad +/- : (only 2D) iteration between face projection 
  - l : console logger on/off
  - t : target atom appear / disappear


Fit to random surface:
We can get coplanarity points where we cant fit plane, then we return a default plane.
The dissemination is running while the sum of distance between the points and our plane dont reach the maximum.


