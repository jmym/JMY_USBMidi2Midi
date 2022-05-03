#include <USB-MIDI.h>
USING_NAMESPACE_MIDI;

typedef USBMIDI_NAMESPACE::usbMidiTransport __umt;
typedef MIDI_NAMESPACE::MidiInterface<__umt> __ss;
__umt usbMIDI(0); // cableNr
__ss MIDICoreUSB((__umt&)usbMIDI);

typedef Message<MIDI_NAMESPACE::DefaultSettings::SysExMaxSize> MidiMessage;

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDICoreSerial);

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void setup()
{
  MIDICoreUSB.setHandleMessage(onUsbMessage);
  MIDICoreSerial.setHandleMessage(onSerialMessage);

  MIDICoreUSB.begin(MIDI_CHANNEL_OMNI);
  MIDICoreSerial.begin(MIDI_CHANNEL_OMNI);
  MIDICoreUSB.turnThruOff();
  MIDICoreSerial.turnThruOff();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void loop()
{
  MIDICoreUSB.read();
  MIDICoreSerial.read();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void onUsbMessage(const MidiMessage& message)
{
  // get the USB MIDI message, defined by these 5 numbers (except SysEX)
    byte type = MIDICoreUSB.getType();
    byte channel = MIDICoreUSB.getChannel();
    byte data1 = MIDICoreUSB.getData1();
    byte data2 = MIDICoreUSB.getData2();
   
   // forward this message to Serial MIDI OUT ports
    if (type != midi::SystemExclusive) {
      // Normal messages, first we must convert usbMIDI's type (an ordinary
      // byte) to the MIDI library's special MidiType.
      midi::MidiType mtype = (midi::MidiType)type;

      // Then simply give the data to the MIDI library send()
      MIDICoreSerial.send(mtype, data1, data2, channel);
      
    } else {
      // SysEx messages are special.  The message length is given in data1 & data2
      unsigned int SysExLength = data1 + data2 * 256;
          //MIDI1.sendSysEx(SysExLength, usbMIDI.getSysExArray(), true);

    }
  //MIDICoreSerial.send(message);
}

void onSerialMessage(const MidiMessage& message)
{

  // get a MIDI IN1 (Serial) message
    byte type = MIDICoreSerial.getType();
    byte channel = MIDICoreSerial.getChannel();
    byte data1 = MIDICoreSerial.getData1();
    byte data2 = MIDICoreSerial.getData2();

  // forward the message to USB MIDI virtual cable #0
    if (type != midi::SystemExclusive) {
      midi::MidiType mtype = (midi::MidiType)type;
      // Normal messages, simply give the data to the usbMIDI.send()
      MIDICoreUSB.send(mtype, data1, data2, channel);
    } else {
      // SysEx messages are special.  The message length is given in data1 & data2
      unsigned int SysExLength = data1 + data2 * 256;
      //usbMIDI.sendSysEx(SysExLength, MIDI1.getSysExArray(), true, 0);
    }
  //MIDICoreUSB.send(message);
}
