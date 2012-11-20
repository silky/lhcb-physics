      subroutine vbf_h_genericpdf(ndns,ih,xmu2,x,fx)
c Interface to vbf_h_MLMPDF package.
      implicit none
      integer ndns,ih
      real * 8 xmu2,x,fx(-6:6)
      real * 4 sxmu2,sx,sfx(-5:5)
      integer j
      sx=x
      sxmu2=xmu2
      call vbf_h_MLMPDF(ndns,ih,sxmu2,sx,sfx,5)
      do j=-5,5
         fx(j)=sfx(j)
      enddo
      fx(1)=sfx(2)
      fx(-1)=sfx(-2)
      fx(2)=sfx(1)
      fx(-2)=sfx(-1)
      fx(6)=0
      fx(-6)=0
      end

c This subroutine is in LHAPDF, and is invoked in
c setstrongcoupl.f in case the lhapdfif.f (and LHAPDF) is linked in
c If not, like now, it should be set to a dummy function to avoid
c link errors. It is never invoked in the present case.
      subroutine vbf_h_getq2min()
      end

      subroutine vbf_h_genericpdfpar(ndns,ih,xlam,scheme,iorder,iret)
      implicit none
      integer ndns,ih
      real * 8 xlam
      character * 2 scheme
      integer iret,iorder
      call vbf_h_PDFPAR(ndns,ih,xlam,scheme,iret)
c not yet implemented
      iorder=-1
      end

      function vbf_h_whichpdfpk()
      character * 3 vbf_h_whichpdfpk
      vbf_h_whichpdfpk='mlm'
      end
