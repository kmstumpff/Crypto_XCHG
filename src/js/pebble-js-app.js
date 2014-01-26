var maxTries = 3;
var numTries = 0;

function sendMessage(price, volume) {
	price   = price || "0.00000000";
	volume   = volume || "0000000000";
	Pebble.sendAppMessage({
		"price": "$" + price,
		"volume": volume,
		},
		function(e){
			console.log("Sent message 1. transactionId=" + e.data.transactionId);
		},
		function(e){
			console.log("Unable to send message. transactionId=" + e.data.transactionId);
			if (numTries < maxTries) {
				numTries++;
				sendMessage(price, volume);
			}
		});
	
}

function returnPriceOfMarket(marketid, response) {
	if (marketid == 3)
		return response.return.markets.LTC.lasttradeprice;
	if (marketid == 132 || marketid == 135)
		return response.return.markets.DOGE.lasttradeprice;
	if (marketid == 29)
		return response.return.markets.NMC.lasttradeprice;
	if (marketid == 28 || marketid == 125)
		return response.return.markets.PPC.lasttradeprice;
	if (marketid == 71 || marketid == 126)
		return response.return.markets.QRK.lasttradeprice;
}

function returnVolumeOfMarket(marketid, response) {
	if (marketid == 3)
		return response.return.markets.LTC.volume;
	if (marketid == 132 || marketid == 135)
		return response.return.markets.DOGE.volume;
	if (marketid == 29)
		return response.return.markets.NMC.volume;
	if (marketid == 28 || marketid == 125)
		return response.return.markets.PPC.volume;
	if (marketid == 71 || marketid == 126)
		return response.return.markets.QRK.volume;
}

function getPrice(marketid) {
	var req = new XMLHttpRequest();
	
	req.open('GET', "http://pubapi.cryptsy.com/api.php?method=singlemarketdata&marketid=" + marketid, true);

	
	req.onload = function(e) {
		if (req.readyState == 4 && req.status == 200) {
			if(req.status == 200) {
				var response = JSON.parse(req.responseText);
				if (response.success == 1) {
					var price = returnPriceOfMarket(marketid, response);
					var volume = parseInt(returnVolumeOfMarket(marketid, response)).toFixed(0).toString();
					//console.log("sending price: " + price);
					//console.log("sending volume: " + volume);
					sendMessage(price, volume);
				} else {
					console.log("Error 1");
				}
			} else {
				console.log("Error 2");
			}
		}
	};
	req.send(null);	
}


Pebble.addEventListener("ready",
						function(e) {
							//console.log(e.type);
							getPrice(3);
                        });
Pebble.addEventListener("appmessage",
						function(e) {
							//console.log(e.type);
							console.log(e.payload.marketid);
							if (e.payload.marketid) {
								getPrice(e.payload.marketid);
							} else {
								console.log("appmessage tansfer error");
							}
						});