module Boutique
	def self.translate_from(bytes)
		# check if this is the right sysex message to listen to
		# pull out p1, p2, v1, v2
		# CS = (100 - ((0x03 + 0x00 + P1 + P2 + V1 + V2) & 0xFF)) & 0x7F
		# bytes = [0xF0, 0x41, 0x10, 0x00, 0x00, 0x00, 0x1C, 0x12, 0x03, 0x00, P1, P2, V1, V2, CS, F7]
		return nil, nil if bytes.length != 16

		controller = ((bytes[10] << 4) | bytes[11]) / 2
		value = ((bytes[12] << 4) | bytes[13]) / 2

		return controller, value
	end

	def self.translate_to(controller, value)
	end
end