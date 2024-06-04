# OpenGLNaveEspacial
Este proyecto se basa en crear una nave espacial funcional con OpenGL y recorer el espacio exterior donde se puede encontrar con objetos espaciales. Para lograrlo primero, he creado tres naves distintas, cada una con cabinas y disparos diferentes. Cada cabina tiene un movimiento específico de la cabeza. Además, he dibujado una zona de aterrizaje donde la nave empieza parada y, al pulsar "P", despega. Si vuelve a estar en la zona de aterrizaje y pulsas "P" de nuevo, aterrizas.

He implementado un cubo invisible que se mueve con la cámara, donde se encuentran todos los asteroides. Si un asteroide sale del cubo, se teletransporta de vuelta al interior del cubo. También he optimizado el uso de polígonos, haciendo que solo se creen los meteoritos visibles.

He diseñado disparos que pueden hacer explotar los asteroides. Para esto, he asignado una ID única a cada asteroide, representada por una tonalidad azul, para identificar el asteroide seleccionado. Redibujo la escena con los asteroides en el backbuffer y leo el píxel de la tonalidad azul para identificarlos. Para encontrar la posición de disparo, leo en el GL_DEPTH_COMPONENT. Una vez tengo la posición y el asteroide, efectúo el disparo. Cuando el disparo alcanza el punto, el asteroide realiza dos animaciones: una en la que aumenta su tamaño hasta teletransportarse a una zona no visible (efecto de destrucción) y otra en la que se crea una nube de polvo.

Para los asteroides, he creado una primitiva propia que replica una esfera punto a punto, con la diferencia de que cada punto se permuta para deformarla. Una vez deformada, puedo escalarla de manera aleatoria para que parezca un asteroide. También he creado mi propia primitiva de esfera para texturizarla correctamente.

Al pulsar "M", se dibujan los planetas del sistema solar alrededor del mapa. Si apuntas hacia uno de ellos, se hace más grande. Una vez seleccionado, al tocar la letra "J", se crea una animación de salto para viajar a ese planeta. Si deseas regresar al hangar, puedes tocar la tecla "H". El fondo es una esfera texturizada por dentro, que se mueve con la cámara.


-------------------------------------------------------------------------------------------------

This project is based on creating a functional spaceship with OpenGL and traversing outer space where it can encounter space objects. To accomplish this first, I created three different ships, each with different cockpits and types of shots. Each cockpit has a specific head movement. In addition, I have drawn a landing zone where the ship starts stationary and, by pressing “P”, takes off. If you return to the landing zone and press “P” again, you land.

I have implemented an invisible cube that moves with the camera, where all the asteroids are located. If an asteroid leaves the cube, it teleports back inside the cube. I have also optimized the use of polygons, making that only visible meteors are created.

I have designed shots that can explode the asteroids. For this, I assigned a unique ID to each asteroid, represented by a blue hue, to identify the selected asteroid. I redraw the scene with the asteroids in the backbuffer and read the blue hue pixel to identify them. To find the firing position, I read in the GL_DEPTH_COMPONENT. Once I have the position and the asteroid, I fire the shot. When the shot reaches the point, the asteroid performs two animations: one in which it increases its size until it teleports to a non-visible area (destruction effect) and another in which a dust cloud is created.

For the asteroids, I have created my own primitive that replicates a sphere point by point, with the difference that each point is permuted to deform it. Once deformed, I can randomly scale it to look like an asteroid. I have also created my own sphere primitive to texture it properly.

Pressing “M” draws the planets of the solar system around the map. If you point at one of them, it becomes larger. Once selected, tapping the letter “J” creates a jump animation to travel to that planet. If you want to return to the hangar, you can tap the “H” key. The background is a textured sphere inside, which moves with the camera.
