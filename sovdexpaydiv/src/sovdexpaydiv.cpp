#include <sovdexpaydiv.hpp>


[[eosio::action]] 
void sovdexpaydiv::initialize(name contract, asset clubstaked){


      queue_table queuetable(get_self(), get_self().value);
      auto existing = queuetable.find(get_self().value);
        
      
      queuetable.emplace( get_self(), [&]( auto& s ) {
      
            s.contract = name{"sovdexpaydiv"};
     
            s.currentpayee = name{"therealgavin"};

            s.remainingpay_gold = asset(0, symbol("XAUT", 4)); 
            s.remainingpay_btc = asset(0, symbol("PBTC", 4)); 
            s.remainingpay_svx = asset(0, symbol("SVX", 4)); 

            s.startpay_gold = asset(0, symbol("XAUT", 4)); 
            s.startpay_btc = asset(0, symbol("PBTC", 4)); 
            s.startpay_svx = asset(0, symbol("SVX", 4));

            s.loadingratio = 1; 
            s.clubstakestart = clubstaked; 
            s.payoutstarttime = current_time_point().sec_since_epoch();
                  
      });

}

[[eosio::action]] 
void sovdexpaydiv::intstaker(name user, asset svxstaked, int laststaketime){


      stake_table staketable(get_self(), get_self().value);

      auto existing = staketable.find(user.value);

      const auto& st = *existing;

      if (existing == staketable.end()){

      
                  staketable.emplace( get_self(), [&]( auto& s ) {

                        s.staker = user;
                        s.svxstaked = svxstaked;
                        s.staketime = laststaketime;

                  });
      }

      else {

                  staketable.modify( st, same_payer, [&]( auto& s ) {

                        s.staker = user;
                        s.svxstaked = svxstaked;
                        s.staketime = laststaketime;

                  });


      }







}






 [[eosio::on_notify("sovdexrelays::minereceipt")]] void sovdexpaydiv::paydiv(name user, asset sovburned, asset minepool){

    

    //1 get current payee
    name payee = get_current_payee();
    //2 see if payee stake time is after start of payout round
    uint32_t userstaketime = get_userstaketime(payee);
    uint32_t roundstarttime = get_payout_start_time();

    if (userstaketime > roundstarttime){

         return;

      }
    
    //3 find current payout amount with payout fraction for user

    
    
    //---------Gold Payout --------------------//
    
    double payratio = get_paying_ratio();

    asset startpay_gold = get_starting_pay_gold();
    asset divpayment_gold = payratio*startpay_gold/100;
    asset minepayment_gold  = divpayment_gold /100;
    
    divpayment_gold = divpayment_gold - minepayment_gold;

    sendasset(payee, divpayment_gold, "Gold Payout 777 Club Member");
    sendasset(user, minepayment_gold, "Gold Payout for Mining");

   
   //-------------BTC Payout --------------------------//


    asset startpay_btc = get_starting_pay_btc();
    asset divpayment_btc = payratio*startpay_btc/100;
    asset minepayment_btc = divpayment_btc /100;


    divpayment_btc = divpayment_btc - minepayment_gold;

    sendasset(payee, divpayment_btc, "BTC Payout 777 Club Member");
    sendasset(user, minepayment_btc, "BTC Payout for Mining");


    //--------------SVX Payout (no Miner SVX) ----------------------//
      
      
    asset startpay_svx = get_starting_pay_svx();
    asset divpayment_svx = payratio*startpay_svx/100;
    
    
    sendasset(payee, divpayment_svx, "SVX Payout 777 Club Member");

    //iterate to next payee
    //check if payee was last payee on the table, if so:
    //1 begin new payment round:
            //a. current payee becomes start of the table
            //b. queue gets 1% moved to active round 
            
    
    
    
   


    


   

 }


[[eosio::on_notify("goldgoldgold::transfer")]] void sovdexpaydiv::insertgold(name from, name to, asset quantity, std::string memo){
      if (from ==get_self() || to != get_self()){
            return;
      }
      update_queue(quantity);

}
[[eosio::on_notify("btcbtcbtcbtc::transfer")]] void sovdexpaydiv::insertbtc(name from, name to, asset quantity, std::string memo){
      if (from ==get_self() || to != get_self()){
            return;
      }
      update_queue(quantity);

}

[[eosio::on_notify("svxmintofeos::transfer")]] void sovdexpaydiv::insertsvx(name from, name to, asset quantity, std::string memo){
      if (from ==get_self() || to != get_self()){
            return;
      }
      update_queue(quantity);

}

[[eosio::on_notify("svxmintofeos::stake")]] void sovdexpaydiv::setstake(name account, asset value){



}


[[eosio::on_notify("svxmintofeos::unstake")]] void sovdexpaydiv::setunstake(name account, asset value){



}