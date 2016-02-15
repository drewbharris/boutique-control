module Boutique
	def self.translate_from(*bytes)
		# check if this is the right sysex message to listen to
		# pull out p1, p2, v1, v2
		# CS = (100 - ((0x03 + 0x00 + P1 + P2 + V1 + V2) & 0xFF)) & 0x7F
		# bytes = [0xF0, 0x41, 0x10, 0x00, 0x00, 0x00, 0x1C, 0x12, 0x03, 0x00, P1, P2, V1, V2, CS, F7]

		controller = ((P1 << 4) | P2) / 2
		value = ((V1 << 4) | V2) / 2

		return controller, value
	end

	def self.translate_to(controller, value)
	end
end