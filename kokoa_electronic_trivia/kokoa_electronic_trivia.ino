#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Configuración del LCD (20 columnas, 4 filas)
// Si 0x27 no funciona, prueba con 0x3F
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Configuración de botones
const int botonIncremento = 7;
const int botonDecremento = 8;
const int botonSeleccionar = 9;

int preguntaActual = 0;
int opcionSeleccionada = 0;
bool opcionConfirmada = false;
bool enSeleccionZona = true;
bool reOrdernarRespuestas = true;
int respuestaCorrecta = 0;
int zonaSeleccion = 0;

// Opciones de zonas de estudio
const char* zonas[3] = {
  "Preguntas de git",
  "Preguntas de bash",
  "Preguntas de linux"
};

// Preguntas y opciones para cada zona
const char* preguntasGit[2] = {"Que es un commit?", "Como crear un branch?"};
const char* opcionesGit[2][3] = {
  {". Guardar cambios", ". Crear rama", ". Fusionar ramas"},
  {". git branch", ". git merge", ". git init"}
};

const char* preguntasBash[2] = {"Que es un script?", "Como listar archivos?"};
const char* opcionesBash[2][3] = {
  {". Programa de comandos", ". Editor de texto", ". Administrador"},
  {". ls", ". cd", ". pwd"}
};

const char* preguntasLinux[2] = {"Como actualizar?", "Que es sudo?"};
const char* opcionesLinux[2][3] = {
  {". apt-get update", ". chmod", ". mv"},
  {". Comando de superusuario", ". Editor de texto", ". Lista de procesos"}
};

// Variables para gestionar las preguntas y respuestas actuales
const char** preguntasActual;
const char*(*opcionesActual)[3];
int totalPreguntas;

void setup() {
  Serial.begin(9600); // Para depuración
  
  // Inicializar LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  
  // Verificar conexión LCD
  lcd.print("Iniciando...");
  delay(1000);
  
  pinMode(botonIncremento, INPUT_PULLUP);
  pinMode(botonDecremento, INPUT_PULLUP);
  pinMode(botonSeleccionar, INPUT_PULLUP);

  mostrarBienvenida();
}

void loop() {
  if (enSeleccionZona) {
    seleccionarZona();
  } else {
    manejarPreguntasYRespuestas();
  }
}

void reOrdenarRespuestasFunc() {
  reOrdernarRespuestas = false;
  //El indice correcto asumiendo que la respuesta correcta siempre es la primera
  int indiceCorrecto = 0;

  for (int i = 2; i > 0; i--) {
    int j = random(0, i+1);
    if (i == indiceCorrecto){
      indiceCorrecto = j;
    }
    else if(j == indiceCorrecto){
      indiceCorrecto = i;
    }
    const char* temp = opcionesActual[preguntaActual][i];
    opcionesActual[preguntaActual][i] = opcionesActual[preguntaActual][j];
    opcionesActual[preguntaActual][j] = temp;
  }
  respuestaCorrecta = indiceCorrecto;
}

void mostrarBienvenida() {
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Bienvenido a");
  lcd.setCursor(4, 1);
  lcd.print("KOKOA Quiz");
  delay(2000);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Presiona cualquier");
  lcd.setCursor(0, 1);
  lcd.print("boton para");
  lcd.setCursor(0, 2);
  lcd.print("continuar :D");
  
  while(digitalRead(botonIncremento) == HIGH && 
        digitalRead(botonDecremento) == HIGH &&
        digitalRead(botonSeleccionar) == HIGH) {
    delay(100); // Esperar hasta que se presione un botón
  }
  
  seleccionarZonaInicial();
}

void seleccionarZonaInicial() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Elige zona de");
  lcd.setCursor(0, 1);
  lcd.print("estudio:");
  delay(1000);
  mostrarZonaActual();
}

void seleccionarZona() {
  if (digitalRead(botonIncremento) == LOW) {
    opcionSeleccionada = (opcionSeleccionada + 1) % 3;
    mostrarZonaActual();
    delay(300);
  }
  
  if (digitalRead(botonDecremento) == LOW) {
    opcionSeleccionada = (opcionSeleccionada - 1 + 3) % 3;
    mostrarZonaActual();
    delay(300);
  }
  
  if (digitalRead(botonSeleccionar) == LOW) {
    confirmarZonaSeleccionada();
    delay(300);
  }
}

void mostrarZonaActual() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Zona:");
  lcd.setCursor(0, 1);
  lcd.print(zonas[opcionSeleccionada]);
}

void confirmarZonaSeleccionada() {
  enSeleccionZona = false;
  zonaSeleccion = opcionSeleccionada;
  switch (opcionSeleccionada) {
    case 0: // Preguntas de git
      preguntasActual = preguntasGit;
      opcionesActual = opcionesGit;
      totalPreguntas = 2;
      break;
    case 1: // Preguntas de bash
      preguntasActual = preguntasBash;
      opcionesActual = opcionesBash;
      totalPreguntas = 2;
      break;
    case 2: // Preguntas de linux
      preguntasActual = preguntasLinux;
      opcionesActual = opcionesLinux;
      totalPreguntas = 2;
      break;
  }
  preguntaActual = 0;
  opcionSeleccionada = 0;
  mostrarPreguntaYOpcion();
}

void manejarPreguntasYRespuestas() {
  if (digitalRead(botonIncremento) == LOW && !opcionConfirmada) {
    opcionSeleccionada = (opcionSeleccionada + 1) % 3;
    mostrarPreguntaYOpcion();
    delay(300);
  }

  if (digitalRead(botonDecremento) == LOW && !opcionConfirmada) {
    opcionSeleccionada = (opcionSeleccionada - 1 + 3) % 3;
    mostrarPreguntaYOpcion();
    delay(300);
  }

  if (digitalRead(botonSeleccionar) == LOW && !opcionConfirmada) {
    opcionConfirmada = true;
    reOrdernarRespuestas = true;
    verificarOpcion();
    delay(300);
  }
}

void mostrarPreguntaYOpcion() {
  if(reOrdernarRespuestas){
    reOrdenarRespuestasFunc();
  }

  lcd.clear();
  // Mostrar la pregunta
  lcd.setCursor(0, 0);
  lcd.print(preguntasActual[preguntaActual]);

  // Mostrar las 3 opciones
  for (int i = 0; i < 3; i++) {
    lcd.setCursor(0, i + 1);
    if (i == opcionSeleccionada) {
      lcd.print("> ");// Flecha más pequeña
      lcd.print(i+1);
    } else {
      lcd.print("  ");// Espacios
      lcd.print(i+1); 
    }
    lcd.print(opcionesActual[preguntaActual][i]);
  }
}

void verificarOpcion() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(esRespuestaCorrecta() ? "Correcto!" : "Incorrecto!");
  delay(1000);

  opcionSeleccionada = 0;
  opcionConfirmada = false;
  if(preguntaActual==1){
    seleccionarZonaInicial();
    reiniciarData();
    return;
  }

  preguntaActual = (preguntaActual + 1) % totalPreguntas;
  mostrarPreguntaYOpcion();
}

bool esRespuestaCorrecta() {
  return opcionSeleccionada == respuestaCorrecta; 
}

void reiniciarData() {
  preguntaActual = 0;
  enSeleccionZona = true;
  reOrdernarRespuestas = true;
    switch (zonaSeleccion) {
    case 0: // Opciones de git
      opcionesGit[0][0] = ". Guardar cambios";
      opcionesGit[0][1] = ". Crear rama";
      opcionesGit[0][2] =". Fusionar ramas";
      opcionesGit[1][0] =". git branch";
      opcionesGit[1][1] =". git merge";
      opcionesGit[1][2] =". git init";
    case 1: // Opciones de bash
    opcionesBash[0][0] = ". Programa de comandos";
    opcionesBash[0][1] =". Editor de texto";
    opcionesBash[0][2] = ". Administrador";
    opcionesBash[1][0] =". ls";
    opcionesBash[1][1] =". cd";
    opcionesBash[1][2] =". pwd";
    case 2: // Opciones de linux
    opcionesLinux[0][0] = ". apt-get update";
    opcionesLinux[0][1] =". chmod";
    opcionesLinux[0][2] =". mv";
    opcionesLinux[1][0] =". Comando de superusuario";
    opcionesLinux[1][1] =". Editor de texto";
    opcionesLinux[1][2] =". Lista de procesos";
  }
  zonaSeleccion = 0;
}