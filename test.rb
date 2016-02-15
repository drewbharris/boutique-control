require "rtmidi"
require_relative "lib/Boutique"

midiin = RtMidi::In.new


##############################################################################
# Boilerplate code for selecting a MIDI port

puts "Available MIDI input ports"
midiin.port_names.each_with_index{|name,index| printf "%3i: %s\n", index, name }

def select_port(midiio)
  print "Select a port number: "
  if (port = gets) =~ /^\d+$/
    return port.to_i if (0...midiio.port_count).include? port.to_i
  end
  puts "Invalid port number"
end

port_index = select_port(midiin) until port_index

##############################################################################
# Use this approach when you need to receive any message including:
# System Exclusive (SysEx), timing, active sensing

midiin.receive_message do |*bytes|
	CS = (100 - ((0x03 + 0x00 + P1 + P2 + V1 + V2) & 0xFF)) & 0x7F
	bytes = [0xF0, 0x41, 0x10, 0x00, 0x00, 0x00, 0x1C, 0x12, 0x03, 0x00, P1, P2, V1, V2, CS, F7]

	controller, value = Boutique.translate_from(bytes)
end

puts "Receiving MIDI messages including SysEx..."
puts "Ctrl+C to exit"

midiin.open_port(port_index)

sleep # prevent Ruby from exiting immediately