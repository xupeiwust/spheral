      subroutine sor_3(group,hoc_points,next_points,
     >     hoc_hoc_up_x,next_hoc_up_x,
     >     hoc_hoc_up_y,next_hoc_up_y,
     >     hoc_hoc_up_z,next_hoc_up_z,
     >     pot,rho,up_x,up_y,up_z,
     >     down_x,down_y,down_z,inside,grav_const,
     >     rjac,eps,debug,maxits,points_maxx)
c
      implicit none
c
      include 'maxx.inc'
c
      integer points_maxx,point,n
      integer group,hoc_points(points_maxx),next_points(points_maxx)
      integer hoc_hoc_up_x,next_hoc_up_x(maxx)
      integer hoc_hoc_up_y,next_hoc_up_y(maxx)
      integer hoc_hoc_up_z,next_hoc_up_z(maxx)
      integer up_x(points_maxx),up_y(points_maxx),up_z(points_maxx)
      integer down_x(points_maxx),down_y(points_maxx)
      integer down_z(points_maxx),point_left,ipass,maxits,n_points
      integer maxits_real
c
      logical inside(points_maxx),debug,it_is_inside
c
      real pot(points_maxx),rho(points_maxx)
      real grav_const,rjac,eps,sixi,anormf,anorm2f
      real omega,anorm,anorm2,resid
c
      sixi=1.0/6.0
      anormf=0.0
      point=hoc_points(group)
      n_points=0
      do while(point .gt. 0)
         if(inside(point)) then
            n_points=n_points+1
            anormf=anormf+abs(pot(point))
         end if
         point=next_points(point)
      end do
c
      anorm2f=3.0*anormf/float(n_points)
c
      maxits_real=maxits/3
      if(n_points .eq. 1) maxits_real=1
c
      omega=1.0
      do n=1,maxits_real
         anorm=0.0
         anorm2=-1.0
         do ipass=1,2
            point_left=hoc_hoc_up_x
            do while(point_left .gt. 0)
               point=point_left
               if(ipass .eq. 2)point=up_x(point)
               do while(it_is_inside(point,inside))
                  resid=
     >                 pot(up_x(point))+pot(up_y(point))+
     >                 pot(up_z(point))+pot(down_x(point))+
     >                 pot(down_y(point))+pot(down_z(point))-
     >                 6.0*pot(point)-grav_const*rho(point)
                  anorm=anorm+abs(resid)
                  anorm2=max(anorm2,abs(resid))
                  pot(point)=pot(point)+omega*resid/6.0
c     
                  point=up_x(point)
                  if(point .gt. 0) point=up_x(point)
               end do
               point_left=next_hoc_up_x(point_left)
            end do
            if(n .eq. 1 .and. ipass .eq. 1) then
               omega=1.0/(1.0-0.5*rjac**2)
            else
               omega=1.0/(1.0-0.25*rjac**2*omega)
            end if
         end do
         if(anorm .lt. eps*anormf) then
           if(anorm2 .lt. eps*anorm2f) then
             print 1,group,n,n_points,
     >         anorm,anormf,anorm2,anorm2f,rjac
             return
           end if
         end if
c
         anorm=0.0
         anorm2=-1.0
         do ipass=1,2
            point_left=hoc_hoc_up_y
            do while(point_left .gt. 0)
               point=point_left
               if(ipass .eq. 2)point=up_y(point)
               do while(it_is_inside(point,inside))
                  resid=
     >                 pot(up_x(point))+pot(up_y(point))+
     >                 pot(up_z(point))+pot(down_x(point))+
     >                 pot(down_y(point))+pot(down_z(point))-
     >                 6.0*pot(point)-grav_const*rho(point)
                  anorm=anorm+abs(resid)
                  anorm2=max(anorm2,abs(resid))
                  pot(point)=pot(point)+omega*resid/6.0
c     
                  point=up_y(point)
                  if(point .gt. 0) point=up_y(point)
               end do
               point_left=next_hoc_up_y(point_left)
            end do
            omega=1.0/(1.0-0.25*rjac**2*omega)
         end do
         if(anorm .lt. eps*anormf) then
           if(anorm2 .lt. eps*anorm2f) then
             print 1,group,n,n_points,
     >         anorm,anormf,anorm2,anorm2f,rjac
             return
           end if
         end if
c
         anorm=0.0
         anorm2=-1.0
         do ipass=1,2
            point_left=hoc_hoc_up_z
            do while(point_left .gt. 0)
               point=point_left
               if(ipass .eq. 2)point=up_z(point)
               do while(it_is_inside(point,inside))
                  resid=
     >                 pot(up_x(point))+pot(up_y(point))+
     >                 pot(up_z(point))+pot(down_x(point))+
     >                 pot(down_y(point))+pot(down_z(point))-
     >                 6.0*pot(point)-grav_const*rho(point)
                  anorm=anorm+abs(resid)
                  anorm2=max(anorm2,abs(resid))
                  pot(point)=pot(point)+omega*resid/6.0
c     
                  point=up_z(point)
                  if(point .gt. 0) point=up_z(point)
               end do
               point_left=next_hoc_up_z(point_left)
            end do
            omega=1.0/(1.0-0.25*rjac**2*omega)
         end do
         if(anorm .lt. eps*anormf) then
           if(anorm2 .lt. eps*anorm2f) then
             print 1,group,n,n_points,
     >         anorm,anormf,anorm2,anorm2f,rjac
             return
           end if
         end if
c
      end do
c
 1    format('sor conv ',3i8,5(1pe12.4))
      if(maxits_real .eq. 1) return
      print*,'not converged'
      stop 'not converged'
      end
