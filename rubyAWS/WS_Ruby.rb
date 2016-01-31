require 'websocket-eventmachine-server'

my_var = "initial"
light = "red"
lock = "locked"
tamper = "ok"

EM.epoll
EM.run do

  trap("TERM") { stop }
  trap("INT")  { stop }
  
    WebSocket::EventMachine::Server.start(:host => "172.31.24.122", :port => 9008) do |ws|

    ws.onopen do
      puts "Client connected on 9008"
	  ws.send "Connected, welcome to Lin's ExPaRiMeNt FOR HACKATHON!!!", :type => "Text"
    end
	
	ws.onmessage do |msg, type|
      puts "\n\nReceived message: #{msg}"
	  puts "The Type is:"
	  puts type
	  
	  ws.send "#{tamper}", :type => type
	  
    end

    ws.onclose do
      puts "Client disconnected at 9008"
    end
  end


  puts "Server started at port 9008"
  
  WebSocket::EventMachine::Server.start(:host => "172.31.24.122", :port => 9007) do |ws|

    ws.onopen do
      puts "Client connected on 9007"
	  ws.send "Connected, welcome to Lin's ExPaRiMeNt FOR HACKATHON!!!", :type => "Text"
    end
	
	ws.onmessage do |msg, type|
      puts "\n\nReceived message: #{msg}"
	  puts "The Type is:"
	  puts type
	  
	  if msg == "ok"
	    tamper = "ok"
	  else msg == "alert"
	    tamper = "alert"
	  end
	  ws.send "tamper Status: #{tamper}", :type => type
	  
    end

    ws.onclose do
      puts "Client disconnected at 9007"
    end
  end


  puts "Server started at port 9007"

  WebSocket::EventMachine::Server.start(:host => "172.31.24.122", :port => 9006) do |ws|

    ws.onopen do
      puts "Client connected on 9006"
	  ws.send "Connected, welcome to Lin's ExPaRiMeNt FOR HACKATHON!!!", :type => "Text"
    end
	
	ws.onmessage do |msg, type|
      puts "\n\nReceived message: #{msg}"
	  puts "The Type is:"
	  puts type
	  
	  ws.send "#{lock}", :type => type
	  
    end

    ws.onclose do
      puts "Client disconnected at 9006"
    end
  end


  puts "Server started at port 9006"
  
  WebSocket::EventMachine::Server.start(:host => "172.31.24.122", :port => 9005) do |ws|

    ws.onopen do
      puts "Client connected on 9005"
	  ws.send "Connected, welcome to Lin's ExPaRiMeNt FOR HACKATHON!!!", :type => "Text"
    end
	
	ws.onmessage do |msg, type|
      puts "\n\nReceived message: #{msg}"
	  puts "The Type is:"
	  puts type
	  
	  if msg == "unlock"
	    lock = "unlocked"
	  end
	  if msg == "lock"
	    lock = "locked"
	  end
	  ws.send "Lock Status: #{lock}", :type => type
	  ws.send "Alarm: #{tamper}", :type => type
    end

    ws.onclose do
      puts "Client disconnected at 9005"
    end
  end


  puts "Server started at port 9005"

  
  WebSocket::EventMachine::Server.start(:host => "172.31.24.122", :port => 9004) do |ws|

    ws.onopen do
      puts "Client connected on 9004"
	  ws.send "Connected, welcome to Lin's ExPaRiMeNt!!!", :type => "Text"
    end
	
	ws.onmessage do |msg, type|
      puts "\n\nReceived message: #{msg}"
	  puts "The Type is:"
	  puts type
	  
	  if msg == "green"
	    light = "green"
	  end
	  ws.send "light color: #{light}", :type => type
	  
    end

    ws.onclose do
      puts "Client disconnected at 9004"
    end
  end


  puts "Server started at port 9004"

  #WebSocket::EventMachine::Server.start(:host => "127.0.0.1", :port => 9001) do |ws|
  WebSocket::EventMachine::Server.start(:host => "172.31.24.122", :port => 9003) do |ws|

    ws.onopen do
      puts "Client connected on 9003"
    end

    ws.onmessage do |msg, type|
      puts "\n\nReceived message: #{msg}"
	  puts "The Type is:"
	  puts type
      ws.send "light color: #{light}", :type => type
    end

    ws.onclose do
      puts "Client disconnected at 9003"
    end

    ws.onerror do |e|
      puts "Error: #{e}"
    end

    ws.onping do |msg|
      puts "Receied ping: #{msg}"
    end

    ws.onpong do |msg|
      puts "Received pong: #{msg}"
    end

  end

  puts "Server started at port 9003"

  def stop
    puts "Terminating WebSocket Server"
    EventMachine.stop
  end

end


#ruby "C:/Users/Lin/Desktop/Hackathon Expariments/WS_Ruby.rb"