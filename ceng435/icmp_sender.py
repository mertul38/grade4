from scapy.all import IP, ICMP, send

# Define the receiver's IP address
receiver_ip = "192.168.1.10"  # Replace with your receiver's actual IP

# Create an ICMP request packet with TTL = 1
icmp_packet = IP(dst=receiver_ip, ttl=1) / ICMP()

# Send the ICMP packet
send(icmp_packet)

print(f"ICMP packet sent to {receiver_ip} with TTL=1")
