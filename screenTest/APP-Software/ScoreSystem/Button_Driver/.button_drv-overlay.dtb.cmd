cmd_/home/stud/beerpong/APP-Software/ScoreSystem/Button_Driver/button_drv-overlay.dtb := mkdir -p /home/stud/beerpong/APP-Software/ScoreSystem/Button_Driver/ ; arm-poky-linux-gnueabi-gcc -E -Wp,-MD,/home/stud/beerpong/APP-Software/ScoreSystem/Button_Driver/.button_drv-overlay.dtb.d.pre.tmp -nostdinc -I./arch/arm/boot/dts -I./arch/arm/boot/dts/include -I./drivers/of/testcase-data -undef -D__DTS__ -x assembler-with-cpp -o /home/stud/beerpong/APP-Software/ScoreSystem/Button_Driver/.button_drv-overlay.dtb.dts.tmp /home/stud/beerpong/APP-Software/ScoreSystem/Button_Driver/button_drv-overlay.dts ; ./scripts/dtc/dtc -O dtb -o /home/stud/beerpong/APP-Software/ScoreSystem/Button_Driver/button_drv-overlay.dtb -b 0 -i /home/stud/beerpong/APP-Software/ScoreSystem/Button_Driver/ -Wno-unit_address_vs_reg -d /home/stud/beerpong/APP-Software/ScoreSystem/Button_Driver/.button_drv-overlay.dtb.d.dtc.tmp /home/stud/beerpong/APP-Software/ScoreSystem/Button_Driver/.button_drv-overlay.dtb.dts.tmp ; cat /home/stud/beerpong/APP-Software/ScoreSystem/Button_Driver/.button_drv-overlay.dtb.d.pre.tmp /home/stud/beerpong/APP-Software/ScoreSystem/Button_Driver/.button_drv-overlay.dtb.d.dtc.tmp > /home/stud/beerpong/APP-Software/ScoreSystem/Button_Driver/.button_drv-overlay.dtb.d

source_/home/stud/beerpong/APP-Software/ScoreSystem/Button_Driver/button_drv-overlay.dtb := /home/stud/beerpong/APP-Software/ScoreSystem/Button_Driver/button_drv-overlay.dts

deps_/home/stud/beerpong/APP-Software/ScoreSystem/Button_Driver/button_drv-overlay.dtb := \

/home/stud/beerpong/APP-Software/ScoreSystem/Button_Driver/button_drv-overlay.dtb: $(deps_/home/stud/beerpong/APP-Software/ScoreSystem/Button_Driver/button_drv-overlay.dtb)

$(deps_/home/stud/beerpong/APP-Software/ScoreSystem/Button_Driver/button_drv-overlay.dtb):
