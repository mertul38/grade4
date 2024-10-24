from scapy.all import sniff, ICMP, IP

# Callback function to process captured packets
def process_packet(packet):
    # Check if it's an ICMP packet and the ICMP type is 8 (Echo Request)
    if ICMP in packet and packet[ICMP].type == 8:  # Type 8 is ICMP Echo Request
        print("ICMP Request Packet Received:")
        packet.show()  # Print the packet details

# Capture ICMP packets (filter ICMP traffic only)
print("Listening for ICMP packets...")
sniff(filter="icmp", prn=process_packet)
