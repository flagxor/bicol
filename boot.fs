( complex core words )
: rot ( abc-bca ) >r swap r> swap ;
: 2rot rot rot ;
: -rot ( abc-cab ) swap >r swap r> ;
: 2drop ( nn- ) drop drop ;
: 2dup ( ab-abab ) over over ;
: 3dup ( abc-abcabc ) >r 2dup r> dup >r -rot r> ;
: invert ( n-n ) -1 xor ;
: negate ( n-n ) invert 1 + ;
: dnegate ( d-d ) invert >r invert 1 um+ r> + ;
: d+ ( dd-d ) >r swap r> um+ r> r> + + ;
: abs ( n-n ) dup 0< ' negate if ;
: zero 0 ;
: minus1 -1 ;
: = ( nn-f ) xor ' zero ' minus1 ifelse ;
: u<-neg swap drop ;
: u<-pos - ;
: u< ( uu-f ) 2dup xor 0< ' u<-neg ' u<-pos ifelse 0< ;
: <-neg drop ;
: <-pos - ;
: < ( nn-f ) 2dup xor 0< ' <-neg ' <-pos ifelse 0< ;
: > ( nn-f ) swap < ;
: do-swap swap ;
: max ( nn-n ) 2dup < ' do-swap if drop ;
: min ( nn-n ) 2dup swap < ' do-swap if drop ;

( exceptions )
user handler
: catch sp@ >r handler @ >r rp@ handler @ execute r> handler ! r> drop 0 ;
: throw handler @ rp! r> handler ! r> swap >r sp! drop r> ;

( os and dynamic linking )
: OS_LINUX 0 ;
: OS_OSX 1 ;
: RTLD_DEFAULT -2 os OS_OSX = and 0 os OS_LINUX = and or ;
: RTLD_LAZY 1 ;
: RTLD_NOW 2 ;
: default-sym drop RTLD_DEFAULT dlsym ;
: dlopen s" dlopen" default-sym call2 ;
: dlclose s" dlclose" default-sym call1 ;

( core io )
: exit s" exit" default-sym call1 ;
: bye 0 exit ;
: write s" write" default-sym call3 ;
: read s" read" default-sym call3 ;
: type >r >r 1 r> r> write drop ;
: emit sp@ 1 type drop ;
: cr 10 emit ;
: key 0 sp@ 0 swap 1 read drop ;

( stdlib )
: NULL 0 ;
: malloc s" malloc" default-sym call1 ;

( sdl variables )
: width 1024 ;
: height 768 ;
user sdl
user window
user renderer
user surface
user texture
user event
user pitch
user pixels

( sdl constants )
: SDL_CENTERED $ff0000 ;
: SDL_INIT_VIDEO $20 ;
: SDL_WINDOW_SHOWN $4 ;
: SDL_WINDOW_RESIZABLE $20 ;
: SDL_RENDERER_ACCELERATED $2 ;
: SDL_RENDERER_PRESENTVSYNC $4 ;
: SDL_QUIT $100 ;
: SDL_KEYDOWN $300 ;
: SDL_KEYUP $301 ;
: SDL_MOUSEMOTION $400 ;
: SDL_MOUSEBUTTONDOWN $401 ;
: SDL_MOUSEBUTTONUP $402 ;

( sdl texture access )
: SDL_TEXTUREACCESS_STATIC 0 ;
: SDL_TEXTUREACCESS_STREAMING 1 ;
: SDL_TEXTUREACCESS_TARGET 2 ;

( sdl blend modes )
: SDL_BLENDMODE_NONE 0 ;
: SDL_BLENDMODE_BLEND 1 ;
: SDL_BLENDMODE_ADD 2 ;
: SDL_BLENDMODE_MOD 4 ;

( sdl functions )
: sdl-dylib-string s" libSDL2.dylib" drop ;
: sdl-so-string s" libSDL2-2.0.so.0" drop ;
: sdl-so sdl-dylib-string os OS_OSX = and sdl-so-string os OS_LINUX = and or ;
: sdl-sym drop sdl @ dlsym ;
: SDL_Init s" SDL_Init" sdl-sym call1 ;
: SDL_CreateWindow s" SDL_CreateWindow" sdl-sym call6 ;
: SDL_CreateRenderer s" SDL_CreateRenderer" sdl-sym call3 ;
: SDL_CreateRGBSurface s" SDL_CreateRGBSurface" sdl-sym call8 ;
: SDL_LockSurface s" SDL_LockSurface" sdl-sym call1 ;
: SDL_UnlockSurface s" SDL_UnlockSurface" sdl-sym call1 ;
: SDL_SetWindowIcon s" SDL_SetWindowIcon" sdl-sym call2 ;
: SDL_SetWindowTitle s" SDL_SetWindowTitle" sdl-sym call2 ;
: SDL_GetWindowPixelFormat s" SDL_GetWindowPixelFormat" sdl-sym call1 ;
: SDL_GetWindowSize s" SDL_GetWindowSize" sdl-sym call3 ;
: SDL_CreateTextureFromSurface s" SDL_CreateTextureFromSurface" sdl-sym call2 ;
: SDL_CreateTexture s" SDL_CreateTexture" sdl-sym call5 ;
: SDL_LockTexture s" SDL_LockTexture" sdl-sym call4 ;
: SDL_UnlockTexture s" SDL_UnlockTexture" sdl-sym call1 ;
: SDL_RenderClear s" SDL_RenderClear" sdl-sym call1 ;
: SDL_RenderCopy s" SDL_RenderCopy" sdl-sym call4 ;
: SDL_RenderPresent s" SDL_RenderPresent" sdl-sym call1 ;
: SDL_SetRenderDrawColor s" SDL_SetRenderDrawColor" sdl-sym call5 ;
: SDL_SetRenderDrawBlendMode s" SDL_SetRenderDrawBlendMode" sdl-sym call2 ;
: SDL_RenderDrawPoints s" SDL_RenderDrawPoints" sdl-sym call3 ;
: SDL_RenderFillRects s" SDL_RenderFillRects" sdl-sym call3 ;
: SDL_GetRendererOutputSize s" SDL_GetRendererOutputSize" sdl-sym call3 ;
: SDL_RestoreWindow s" SDL_RestoreWindow" sdl-sym call1 ;
: SDL_PollEvent s" SDL_PollEvent" sdl-sym call1 ;
: SDL_FreeSurface s" SDL_FreeSurface" sdl-sym call1 ;
: SDL_DestroyTexture s" SDL_DestroyTexture" sdl-sym call1 ;
: SDL_DestroyRenderer s" SDL_DestroyRenderer" sdl-sym call1 ;
: SDL_DestroyWindow s" SDL_DestroyWindow" sdl-sym call1 ;
: SDL_Quit s" SDL_Quit" sdl-sym call0 ;

( sdl events )
: event-type event @ l@ ;
: key-code event @ 16 + s@ ;
: key-sym event @ 16 + 4 + s@ ;
: SDL_QUIT $100 ;
: SDL_KEYDOWN $300 ;
: SDL_KEYUP $301 ;

( random )
user randa
user randb
user randc
: rand 123457 randa +! randa @ 123 * 7 +
  -- randb +! randb @ 77 + randc +! randc @ ;
: rand-unit rand $fff and $7ff - ;
: rand-vec rand-unit rand-unit rand-unit ;
: square dup * ;
: length2 square swap square + swap square + ;
: outside-sphere length2 $3fffff > ;
: try-rand-sphere drop drop drop rand-vec 3dup outside-sphere 0 = brif ;
: rand-sphere 0 0 0 ' try-rand-sphere repeat forever ;

( particles )
: particle-count 20000 ;
user wwidth
user wheight
user particles
user position
user velocity
: alloc-particles
  -- particle-count 4 * 4 * malloc particles !
  -- particle-count cells 3 * malloc position !
  -- particle-count cells 3 * malloc velocity ! ;
: init-one-pos
  -- 128 12 lshift rand-unit + over ! cell+
  -- 200 12 lshift rand-unit + over ! cell+
  -- 128 12 lshift rand-unit + over ! cell+ ;
: init-one-partial
  -- cell+
  -- 200 12 lshift rand-unit + over ! cell+
  -- cell+ ;
: init-one-vel rand-sphere >r $fff - >r
  -- over ! cell+ r> over ! cell+ r> over ! cell+ ;

( particle modes )
user particle-mode
: particle-mode1 init-one-partial swap init-one-vel swap ;
: particle-mode2 init-one-pos swap init-one-vel swap ;
: particle-mode3 init-one-pos ;
: particle-mode4 swap init-one-vel swap ;
: particle-mode5 ;

( rest of particle init )
: init-particles
  -- position @ particle-count ' init-one-pos for next drop
  -- velocity @ particle-count ' init-one-vel for next drop
  -- ;
: copy-one
  -- 2dup @ wwidth l@ * 20 rshift swap l! cell+ swap 4 + swap
  -- 2dup @ wheight l@ * 20 rshift swap l! cell+ swap 4 + swap
  -- cell+
  -- over 20 swap l! swap 4 + swap
  -- over 20 swap l! swap 4 + swap ;
: copy-particles
  -- window @ wwidth wheight SDL_GetWindowSize drop
  -- particles @ position @ particle-count
  -- ' copy-one for next
  -- drop drop
  -- ;
: tick-one 2dup @ swap @ + over ! cell+ swap cell+ swap ;
: do-bounce dup @ abs negate 15 * 4 arshift swap ! ;
: skip-bounce drop ;
: bounce-one cell+ swap cell+ swap
  -- 2dup @ $10f000 >
  -- ' do-bounce ' skip-bounce ifelse
  -- cell+ swap cell+ swap
  -- cell+ swap cell+ swap ;
: gravity-one cell+ dup 20 swap +! cell+ cell+ ;
: damp-one dup @ 255 * 1 + 8 arshift over ! cell+ ;
: do-reset particle-mode @ execute ;
: skip-reset 3 cells + swap 3 cells + swap ;
: reset-one rand 255 and 5 < ' do-reset ' skip-reset ifelse ;
: tick-particles
  -- velocity @ position @ particle-count 3 * ' tick-one for next
  -- drop drop
  -- velocity @ position @ particle-count ' bounce-one for next
  -- drop drop
  -- velocity @ position @ particle-count ' reset-one for next
  -- drop drop
  -- velocity @ particle-count ' gravity-one for next
  -- drop
  -- velocity @ particle-count 3 * ' damp-one for next
  -- drop
  -- copy-particles
  -- ;

( sdl test )
: solid $ff0000 $100 * ;
user point
: row pitch @ * pixels @ + point ! ;
: nextp 4 point +! ;
user offset
: plot offset @ * solid or point @ l! nextp ;
: draw-pixels-one 2dup 2dup >r >r 256 * + 256 * r> * r> + plot 1 + ;
: draw-pixels-row dup row 0 ' draw-pixels-one for next drop 1 + ;
: draw-pixels 0 height ' draw-pixels-row for next drop ;
: draw
  -- texture @ NULL pixels pitch SDL_LockTexture drop
  -- pitch l@ pitch !
  -- draw-pixels
  -- texture @ SDL_UnlockTexture drop
  -- ;
: pixel-format window @ SDL_GetWindowPixelFormat ;
: setup-sdl
  -- 512 malloc event !
  -- alloc-particles
  -- init-particles
  -- ' particle-mode5 particle-mode !
  -- sdl-so RTLD_LAZY dlopen sdl !
  -- SDL_INIT_VIDEO SDL_Init drop
  -- s" Forth" drop SDL_CENTERED SDL_CENTERED width height
  -- SDL_WINDOW_SHOWN SDL_WINDOW_RESIZABLE or SDL_CreateWindow window !
  -- window @ -1 SDL_RENDERER_ACCELERATED SDL_RENDERER_PRESENTVSYNC or
  -- SDL_CreateRenderer renderer !
  -- 0 width height 32 $ff $ff00 $ff0000 $ff0000 $100 *
  -- SDL_CreateRGBSurface surface !

( -- window @ surface @ SDL_SetWindowIcon drop )
  -- window @ s" Forth Test" drop SDL_SetWindowTitle drop
  -- renderer @ pixel-format SDL_TEXTUREACCESS_STREAMING width height
  -- SDL_CreateTexture texture !
  -- ;
: key-q 1 rdrop rdrop ;
: key-1 ' particle-mode1 particle-mode ! ;
: key-2 ' particle-mode2 particle-mode ! ;
: key-3 ' particle-mode3 particle-mode ! ;
: key-4 ' particle-mode4 particle-mode ! ;
: key-5 ' particle-mode5 particle-mode ! ;
: key-space init-particles ;
: handle-key
  -- key-sym 113 = ' key-q if
  -- key-sym 49 = ' key-1 if
  -- key-sym 50 = ' key-2 if
  -- key-sym 51 = ' key-3 if
  -- key-sym 52 = ' key-4 if
  -- key-sym 53 = ' key-5 if
  -- key-sym 32 = ' key-space if
  -- ;
: quit 1 rdrop ;
: sdl-events
  -- event @ SDL_PollEvent drop
  -- event-type SDL_QUIT = ' quit if
  -- event-type SDL_KEYDOWN = ' handle-key if
  -- 0 ;
: sdl-step
  -- sdl-events
  -- tick-particles
  -- renderer @ 0 0 30 255 SDL_SetRenderDrawColor drop
  -- renderer @ SDL_RenderClear drop
  -- renderer @ SDL_BLENDMODE_ADD SDL_SetRenderDrawBlendMode drop
  -- renderer @ 128 80 20 20 SDL_SetRenderDrawColor drop

( -- renderer @ particles @ particle-count SDL_RenderDrawPoints drop )
  -- renderer @ particles @ particle-count SDL_RenderFillRects drop
  -- renderer @ SDL_RenderPresent drop
  -- brif ;
: sdl-cleanup
  -- texture @ SDL_DestroyTexture drop
  -- surface @ SDL_FreeSurface drop
  -- renderer @ SDL_DestroyRenderer drop
  -- window @ SDL_DestroyWindow drop
  -- SDL_Quit drop
  -- ;
: sdl-loop ' sdl-step repeat forever ;
: sdl-run setup-sdl sdl-loop sdl-cleanup ;

( boot up )
: test s" Hello Forth!" type cr ;
: test2a dup emit 1 + ;
: test2 65 26 ' test2a for next cr ;
: console-tests test ' test2 execute ;
: boot console-tests sdl-run bye ;

( boot it up )
  -- boot
