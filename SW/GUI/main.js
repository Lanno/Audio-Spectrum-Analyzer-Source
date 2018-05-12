$(function() {
    $.plot("#time_domain", [0,0]);

    $.plot("#freq_domain", [0,0]);
    
    setInterval(function() {
        var message = Queue_Wrapper.message();
        
        if(message !== "empty") {
            var elem = $("<li></li>")
                .append(message);
        
            $("#messages").append(elem);
            
        }				
    }, 1000);
    
    setInterval(function() {
        var serial_data = Queue_Wrapper.data(); 
        
        if(serial_data !== "empty") {
            var data = JSON.parse(serial_data);
            
            var left_pairs  = [];
            
            var right_pairs = [];
            
            for(var idx = 0; idx < data.length; idx++) {
                var left  = (data[idx] & 0xffff0000) >> 16;
                
                var right =  data[idx] & 0x0000ffff;
                
                if(left >= (1 << 15)) {
                    left = 0xffff0000 | left;
                }
                
                if(right >= (1 << 15)) {
                    right = 0xffff0000 | right;
                }
                
                left  = left  / Math.pow(2.0, 16);
                
                right = right / Math.pow(2.0, 16);
                
                left_pairs.push([idx, left]);
                
                right_pairs.push([idx, right]);
            }

            $.plot("#time_domain", [left_pairs, right_pairs]);
            
        }
    }, 1000);
});