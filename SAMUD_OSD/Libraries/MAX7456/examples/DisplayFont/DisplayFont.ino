/*
  DisplayFont.ino
    
    Writes the full MAX7456 character table onto the video.


    This example code is in the public domain.
    Requires Arduino 1.0 or later.


    History :
      1.0 ~ Creation of MAX7456Demo from MAX7456Demo 0.4
            2013.01.31 - F. Robert Honeyman <www.coldcoredesign.com>
      
*/



// Included Libraries //////////////////////////////////////////////////////////

  #include <SPI.h>
  #include <MAX7456.h>





// Pin Mapping /////////////////////////////////////////////////////////////////
  
  // pinValue = 0 means "not connected"

  //  FDTI Basic 5V                   ---  Arduino  VCC      (AVCC,VCC)
  //  FDTI Basic GND                  ---  Arduino  GND      (AGND,GND)
  //  FDTI Basic CTS                  ---  Arduino  GND      (AGND,GND)
  //  FDTI Basic DTR                  ---  Arduino  GRN
  //  FDTI Basic TXO                  ---> Arduino  TXO [PD0](RXD)
  //  FDTI Basic RXI                 <---  Arduino  RXI [PD1](TXD)
  
  
  //  Max7456 +5V   [DVDD,AVDD,PVDD]  ---  Arduino  VCC      (AVCC,VCC)
  //  Max7456 GND   [DGND,AGND,PGND]  ---  Arduino  GND      (AGND,GND)
  //  Max7456 CS    [~CS]            <---  Arduino  10  [PB2](SS/OC1B)
  //  Max7456 CS    [~CS]            <---  Mega2560 43  [PL6]
  const byte osdChipSelect             =            43;
  //  Max7456 DIN   [SDIN]           <---  Arduino  11  [PB3](MOSI/OC2)
  //  Max7456 DIN   [SDIN]           <---  Mega2560 51  [PB2](MOSI)
  const byte masterOutSlaveIn          =                      MOSI;
  //  Max7456 DOUT  [SDOUT]           ---> Arduino  12  [PB4](MISO)
  //  Max7456 DOUT  [SDOUT]           ---> Mega2560 50  [PB3](MISO)
  const byte masterInSlaveOut          =                      MISO;
  //  Max7456 SCK   [SCLK]           <---  Arduino  13  [PB5](SCK)
  //  Max7456 SCK   [SCLK]           <---  Mega2560 52  [PB1](SCK)
  const byte slaveClock                =                      SCK;
  //  Max7456 RST   [~RESET]          ---  Arduino  RST      (RESET)
  const byte osdReset                  =            0;
  //  Max7456 VSYNC [~VSYNC]          -X-
  //  Max7456 HSYNC [~HSYNC]          -X-
  //  Max7456 LOS   [LOS]             -X-





// Global Macros ///////////////////////////////////////////////////////////////
  
  //#define DEBUG true





// Global Constants ////////////////////////////////////////////////////////////

  const unsigned long debugBaud = 9600;         // Initial serial baud rate for 
                                                //   Debug PC interface
  




// Global Variables ////////////////////////////////////////////////////////////
  
  HardwareSerial Debug = Serial;                // Set debug connection
  
  MAX7456 OSD( osdChipSelect );





// Hardware Setup //////////////////////////////////////////////////////////////

  void setup() {
    
    // Initialize the Serial connection to the debug computer:
    Debug.begin( debugBaud );
    

    // Initialize the SPI connection:
    SPI.begin();
    SPI.setClockDivider( SPI_CLOCK_DIV2 );      // Must be less than 10MHz.
    
    // Initialize the MAX7456 OSD:
    OSD.begin();                                // Use NTSC with default area.
    OSD.setSwitchingTime( 5 );                  // Set video chroma distortion 
                                                //   to a minimum.
    OSD.display();                              // Activate the text display.
    OSD.setCharEncoding(MAX7456_ASCII);         // Use non-decoded access.
    OSD.noLineWrap();                           // Set wrap behaviour.
    OSD.noPageWrap();
    
    
  } // setup()





// Main Code ///////////////////////////////////////////////////////////////////

  void loop() {
    
    // Instantiate local variables:
    int iDelay = 150;
    
    
    
    // Change layout of screen to demonstrate sizing and alignment.
    writeFont();
    while (true)
    {
      
      for (int iRow = OSD.rows(); 
           iRow >= 1; 
           iRow--                )
      {
        while ( OSD.notInVSync() );
        OSD.setTextArea( OSD.columns(), iRow );
        writeFont();
        delay( iDelay * 2 );
      }
      
      for (int iCol = OSD.columns(); 
           iCol >= 1; 
           iCol--                   )
      {
        while ( OSD.notInVSync() );
        OSD.setTextArea( iCol, OSD.rows() );
        writeFont();
        delay( iDelay );
      }
      
      for (int iRow = 1; 
           iRow <= MAX7456_ROWS_N0; 
           iRow++                  )
      {
        while ( OSD.notInVSync() );
        OSD.setTextArea( OSD.columns(), iRow );
        writeFont();
        delay( iDelay * 2 );
      }
      
      for (int iCol = 1; 
           iCol <= MAX7456_COLS_N0; 
           iCol++                  )
      {
        while ( OSD.notInVSync() );
        OSD.setTextArea( iCol, OSD.rows() );
        writeFont();
        delay( iDelay );
      }
      
    }
    
  } // loop()





// Interrupt Methods ///////////////////////////////////////////////////////////





// Local Methods ///////////////////////////////////////////////////////////////

  void writeFont() {
    
    // Instantiate local variables:
    byte ubSymbol = 0;

    
    // Loop through symbols and display them:
    while (OSD.notInVSync());                   // Wait for VSync to start
    OSD.clear();                                // Clear display
    
    for (int iRow = 0;                          // Write the last 80 symbols 
         iRow < 0 + 5;                          //   in two blocks, writing
         iRow++       )                         //   40 in the leftmost block
    {
      OSD.setCursor( -8, iRow );
      for (int iCol = -8;                       // and let it contain 5 rows 
           iCol < (-8) + 8;                     //   of 8 columns each, 
           iCol++)                              //   starting at (-8, 0)
      {
        ubSymbol = (iRow - 0 + 11) * 16         // given the row and column, 
                       + (iCol - (-8) + 0);     //   derive symbol index
        OSD.write( ubSymbol );                  // write symbol to display
      }
    }
    
    for (int iRow = 0;                          // Write the first 112 symbols 
         iRow < 7;                              //   in a block of 7 rows
         iRow++       )
    {
      OSD.setCursor( 0, iRow );
      for (int iCol = 0;                        // by 16 columns, starting at 
           iCol < 16;                           //   (0, 0)
           iCol++       )
      {
        ubSymbol = iRow * 16                    // given the row and column, 
                       + iCol;                  //   derive symbol index
        OSD.write( ubSymbol );                  // write symbol to display
      }
    }
    
    for (int iRow = -4;                         // Write the mid 64 symbols 
         iRow < -4 + 4;                         //   in a block of 4 rows
           iRow++        )
    {
      OSD.setCursor( 0, iRow );
      for (int iCol = 0;                        // by 16 columns, starting at 
           iCol < 16;                           //   (0, 0)
           iCol++       )
      {
        ubSymbol = (iRow - (-4) + 7) * 16       // given the row and column, 
                       + iCol;                  //   derive symbol index
        OSD.write( ubSymbol );                  // write symbol to display
      }
    }
    
    for (int iRow = -5;                         // Write the last 80 symbols 
         iRow < -5 + 5;                         //   in two blocks, writing
         iRow++        )                        //   40 in the rightmost block
    {
      OSD.setCursor( -8, iRow );
      for (int iCol = -8;                       // and let it contain 5 rows 
           iCol < (-8) + 8;                     //   of 8 columns each, 
           iCol++          )                    //   starting at (-8, -5)
      {
        ubSymbol = (iRow - (-5) + 11) * 16      // given the row and column, 
                       + (iCol - (-8) + 8);     //   derive symbol index
        OSD.write( ubSymbol );                  // write symbol to display
      }
    }
    
    OSD.display();                              // Display text
    while (!OSD.notInVSync());                  // Wait for VSync to finish
    
  } // writeFont()
  
  
  
